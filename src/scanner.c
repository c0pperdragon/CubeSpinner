
// ------------ PERFORM FACEKET COLOR SCAN -------

#define COLOR_YELLOW  0
#define COLOR_RED     1
#define COLOR_BLUE    2
#define COLOR_WHITE   3
#define COLOR_ORANGE  4
#define COLOR_GREEN   5
#define COLOR_UNKNOWN 6
#define COLOR_NOSCAN  7

char colorletters[] = { 'Y', 'R', 'B', 'W', 'O', 'G', '?', '.'};
int edgefacelets[12][2] = { {1,46}, {5,10}, {7,19}, {3,37},
                            {21,41}, {23,12}, {14,48}, {50,39},
                            {28,25}, {32,16}, {34,52}, {30,43} };

#define NUMFACES (6*9)
int facergb[NUMFACES][3];
byte facecolors[NUMFACES];

int havecalibratedtwosensors = 0;
int whitecalibration[2][3];


// transformations from rgb space to colors

int max(int a, int b)
{
    return a>b ? a:b;
}
int min(int a, int b)
{
    return a<b ? a:b;
}

int compute_h(int* rgb)
{
    int R = rgb[0];
    int G = rgb[1];
    int B = rgb[2];
    int MAX = max(max(R,G),B);
    int MIN = min(min(R,G),B);
    if (MAX==MIN)
    {   return 0;
    }
    int h = 0;
    if (MAX==R)
    {   h=  (60*(G-B))/(MAX-MIN);
    }
    else if (MAX==G)
    {   h = 120+(60*(B-R))/(MAX-MIN);
     }
    else if (MAX==B)
    {   h =  240+(60*(R-G))/(MAX-MIN);
    }
    if (h<0)
    {  return h+360;
    }
    if (h>=360)
    {  return h-360;
    }
    return h;
}

int compute_s(int* rgb)
{
    int R = rgb[0];
    int G = rgb[1];
    int B = rgb[2];
    int MAX = max(max(R,G),B);
    int MIN = min(min(R,G),B);
    if (MAX==0)
    {   return 0;
    }
    return (100*(MAX-MIN))/MAX;
}


int is_more_orange(int* rgb1, int *rgb2)
{
    int score1 = ((compute_h(rgb1)+180)%360) - compute_s(rgb1);
    int score2 = ((compute_h(rgb2)+180)%360) - compute_s(rgb2);
    if (score1>score2)
    {   return score1-score2;
    }
    else
    {   return 0;
    }
}


void print_side(int x, int y, int startfacelet)
{
    int i;
    char buffer[10];
    for (i=0; i<9; i++)
    {   sprintf(buffer, "%c", colorletters[facecolors[startfacelet+i]]);
        LcdText(1, x+(i%3)*14,y+(i/3)*14, buffer);
    }
}

// scanning of facelets on one side and store as rgb values
// return: true, if second scanner was detected
int turn_cw_while_scan(int numscan, int extraturn, int* targets1, int *targets2, int plotcolorchannels)
{
    short samples1[500][3];
    int numsamples1;
    short samples2[500][3];
    int numsamples2;
    long sampleread_start;
    long sampleread_end1;
    long sampleread_end2;
    long facelettimestamp[8];
    int faceletsfound = 0;

    // do not start while motors are still busy
    waitwhilebusy(OUT_AD);

    // determine where the position of the center of the first facelet is
    int startposition;
    OutputGetCount(OUT_A, &startposition);
    int prevposition = startposition;
    long prevpositiontime = TimerGetMS();
    int facelet0center = startposition - 3;

    // start turning the cube
    OutputPolarity(OUT_AD,-1);
    OutputStepPowerEx(OUT_AD, 100, 0, (numscan+extraturn)*45, 0, TRUE, OWNER_NONE);

    // start to read new samples (and get time)
    sampleread_start = sampleread_end1 = sampleread_end2 = TimerGetMS();
    numsamples1 = 0;
    numsamples2 = 0;
    SensorUARTDiscardInputBuffer(0);
    SensorUARTDiscardInputBuffer(3);

    // as long as cube is rotating, keep scanning and
    // get timestamps for the scan positions
    for (;;)
    {   long now = TimerGetMS();

        // collect all samples that are there (and memorize when latest sample was taken)
        while (numsamples1<500)
        {   if(SensorUARTGetShorts(0, samples1[numsamples1], 3))
            {   numsamples1++;
                sampleread_end1 = now;
            }
            else
            {   break;
            }
        }
        while (numsamples2<500)
        {   if(SensorUARTGetShorts(3, samples2[numsamples2], 3))
            {   numsamples2++;
                sampleread_end2 = now;
            }
            else
            {   break;
            }
        }

        // check if the next scan points is reached by the motor
        // and memorize timestamp
        int position;
        OutputGetCount(OUT_A, &position);
        // wait for a jump in the position - only then exact values can be expected
        if (position!=prevposition)
        {   if (faceletsfound<numscan)
            {   int scanpoint = facelet0center - 45*faceletsfound;
                if (position<=scanpoint)
                {   // interpolate the time when the position crossed the scan-point
                    facelettimestamp[faceletsfound] =
                        prevpositiontime +
                        (now-prevpositiontime) * (scanpoint-prevposition) / (position-prevposition);
                    faceletsfound++;
                }
            }
            prevposition = position;
            prevpositiontime = now;
        }

        // check if turning can already be stopped to allow
        // fast follow-up twisting of sides (this does not wait for the extra turn to finish)
        int targeta = rounded_90(startposition)-numscan*45;
        if (position<=targeta+25)
        {   break;
        }

        // give other parts of the system time to do their job
        /// (e.g. put new motor position into shared memory region)
        Wait(1);
    }

    // if needed, plot the scan samples as a graph
    if (plotcolorchannels)
    {   int x,c;
        for (x=0; x<numsamples1 && x<178; x++)
        {   for (c=0; c<3; c++)
            {   if (plotcolorchannels & (1<<c))
                {   int y = 125 - samples1[x][c];
                    DisplaySetPixel(x, y);
                }
            }
        }
        for (x=0; x<numsamples2 && x<178; x++)
        {   for (c=0; c<3; c++)
            {   if (plotcolorchannels & (0x10<<c))
                {   int y = 125 - samples2[x][c];
                    DisplaySetPixel(x, y);
                }
            }
        }
    }

    // extract color for all facelets
    int f;
    for (f=0; f<faceletsfound && sampleread_end1>sampleread_start; f++)
    {   // determine the mapping of the facelets to the samples
        int idx = ((facelettimestamp[f] - sampleread_start) * numsamples1) / (sampleread_end1-sampleread_start);

        // if log is needed, add the sampling point to the graph
        if (plotcolorchannels & 7)
        {   int x = idx;
            if (x<178)
            {  int y;
              for (y=0; y<128; y++)
              {  DisplaySetPixel (x,y);
              }
            }
        }
        // extract color values to global list
        if (idx<1)
        { idx = 1;
        }
        if (idx>numsamples1-2)
        {   idx=numsamples1-2;
        }
        if (targets1)
        {   int t = targets1[f];
          // take sum of 3 samples
          facergb[t][0] = (samples1[idx-1][0]+samples1[idx][0]+samples1[idx+1][0]);
          facergb[t][1] = (samples1[idx-1][1]+samples1[idx][1]+samples1[idx+1][1]);
          facergb[t][2] = (samples1[idx-1][2]+samples1[idx][2]+samples1[idx+1][2]);
        }
    }
    for (f=0; f<faceletsfound && sampleread_end2>sampleread_start; f++)
    {   // determine the mapping of the facelets to the samples
        int idx = ((facelettimestamp[f] - sampleread_start) * numsamples2) / (sampleread_end2-sampleread_start);
        // extract color values to global list and apply normalization for R,G,B on the way

        // if log is needed, add the sampling point to the graph
        if (plotcolorchannels & 0x70)
        {   int x = idx;
            if (x<178)
            {  int y;
              for (y=0; y<128; y++)
              {  DisplaySetPixel (x,y);
              }
            }
        }
        // extract color values to global list
        if (idx<1)
        { idx = 1;
        }
        if (idx>numsamples2-2)
        {   idx=numsamples2-2;
        }
        if (targets2)
        {   int t = targets2[f];
          // take sum of 3 samples
          facergb[t][0] = (samples2[idx-1][0]+samples2[idx][0]+samples2[idx+1][0]);
          facergb[t][1] = (samples2[idx-1][1]+samples2[idx][1]+samples2[idx+1][1]);
          facergb[t][2] = (samples2[idx-1][2]+samples2[idx][2]+samples2[idx+1][2]);
        }
    }

    return numsamples2>10;
}

void copyrgb(int *rgb_from, int* rgb_to)
{
    rgb_to[0] = rgb_from[0];
    rgb_to[1] = rgb_from[1];
    rgb_to[2] = rgb_from[2];
}


void calibrate24colors(int* white, int* facelets)
{
    int i,c;
    for (i=0; i<24; i++)
    {   int *rgb = facergb[facelets[i]];
        for (c=0; c<3; c++)
        {   rgb[c] = (rgb[c] * 1000) / white[c];
        }
    }
}

// scramble the cube while scanning all faces
// at start, the B side is inside the twister and the process also
// stops with this orientation
bool scan_all_sides()
{
    // clear all rgb values (all values 0)
    int i;
    for (i=0; i<NUMFACES; i++)
    {   facergb[i][0] = facergb[i][1] = facergb[i][2] = 0;
    }

    // try the first scan of the fast variant to also determine if there are two sensors available
    int twosensors = turn_cw_while_scan(2,0, (int[]){21,42}, (int[]){50,15}, 0);

    // when there are two sensors and the calibration was not done, do it now
    // this leads to an invalid scan result for this time, but the cube is
    // restored, so the user can do a real fast solve then
    if (twosensors && !havecalibratedtwosensors)
    {   for (i=0; i<3; i++)
        {   whitecalibration[0][i] = whitecalibration[1][i] = 0;
        }
        for (i=0; i<6; i++)
        {   // scan whole side (at first re-use present data)
            if (i==0)
            {   copyrgb (facergb[21],facergb[0]);
                copyrgb (facergb[42],facergb[1]);
                copyrgb (facergb[50],facergb[10]);
                copyrgb (facergb[15],facergb[11]);
                turn_cw_while_scan(6,0, (int[]){2,3,4,5,6,7,8}, (int[]){12,13,14,15,16,17,18}, 0);
            }
            else
            {   turn_cw_while_scan(8,0, (int[]){0,1,2,3,4,5,6,7,8}, (int[]){10,11,12,13,14,15,16,17,18}, 0);
            }
            // check if any color is brighter then current white calibration
            int j;
            for (j=0; j<9; j++)
            {   if (facergb[j][0]+facergb[j][1]+facergb[j][2] > whitecalibration[0][0]+whitecalibration[0][1]+whitecalibration[0][2])
                {   copyrgb(facergb[j],whitecalibration[0]);
                }
            }
            for (j=10; j<19; j++)
            {   if (facergb[j][0]+facergb[j][1]+facergb[j][2] > whitecalibration[1][0]+whitecalibration[1][1]+whitecalibration[1][2])
                {   copyrgb(facergb[j],whitecalibration[1]);
                }
            }
            // twist sides to bring other pieces to top (after 6 passes, cube is in original state)
            twistside_cw();
            turn_180();
            twistside_ccw();
            turn_cw();
            if (j%1)
            {  twistside_cw();
            }
            else
            {  twistside_ccw();
            }
            turn_180();
            if (j%1)
            {  twistside_ccw();
            }
            else
            {  twistside_cw();
            }
            turn_ccw();
        }
        havecalibratedtwosensors = true;
        return false;          // no real scan - must re-do
    }

    // continue fast operation with two sensors
    else if (twosensors)
    {
        twistside_cw();
        turn_cw_while_scan(2,0, (int[]){43,6}, (int[]){12,27}, 0);
        twistside_ccw();
        turn_cw_while_scan(4,0, (int[]){25,45,37,51}, (int[]){52,20,14,26}, 0);
        twistside_cw();
        turn_cw_while_scan(2,0, (int[]){19,18}, (int[]){46,47}, 0);
        twistside_cw();
        turn_cw_while_scan(4,0, (int[]){10,24,25,8}, (int[]){41,53,52,29}, 0);
        twistside_ccw();
        turn_cw_while_scan(2,0, (int[]){16,17}, (int[]){39,36}, 0);
        twistside_ccw();
        turn_cw_while_scan(4,0, (int[]){3,11,10,9}, (int[]){30,38,41,44}, 0);
        twistside_cw();
        turn_cw_while_scan(2,0, (int[]){48,51}, (int[]){23,26}, 0);
        twistside_cw();
        turn_cw_while_scan(8,2, (int[]){1,0,3,6,7,8,5,2}, (int[]){34,33,30,27,28,29,32,35}, 0);

        //                              not scanned: 4,13,22,31,40,49
        calibrate24colors(whitecalibration[0], (int[]){0,1,2,3,5,6,7,8,9,10,11,16,17,18,19,21,24,25,37,42,43,45,48,51});
        calibrate24colors(whitecalibration[1], (int[]){12,14,15,20,23,26,27,28,29,30,32,33,34,35,36,38,39,41,44,46,47,50,52,53});
        return true;
    }
    // slow operation with one sensor only
    else
    {
      // re-use info of already scanned faces, but put into other target positions,
      // because the scan sequence is different
      copyrgb (facergb[21],facergb[50]);
      copyrgb (facergb[42],facergb[11]);
      // continue with slow scan
      turn_cw_while_scan(4,0, (int[]){10,9,48,15}, 0,0);
      twistside_cw();
      turn_cw_while_scan(4,0, (int[]){16,17,50,11}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(2,0, (int[]){10,6}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(4,0, (int[]){25,8,16,2}, 0,0);
      twistside_cw();
      turn_cw_while_scan(6,0, (int[]){19,18,12,24,25,26}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(4,0, (int[]){14,20,19,18}, 0,0);
      twistside_cw();
      turn_cw_while_scan(2,0, (int[]){12,15}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(4,0, (int[]){30,17,14,11}, 0,0);
      twistside_cw();
      turn_cw_while_scan(6,0, (int[]){32,29,28,27,30,33}, 0,0);
      twistside_cw();
      turn_cw_while_scan(4,0, (int[]){34,35,32,29}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(2,0, (int[]){28,18}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(4,0, (int[]){21,24,34,26}, 0,0);
      twistside_cw();
      turn_cw_while_scan(6,0, (int[]){23,38,37,36,21,42}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(4,0, (int[]){43,44,23,38}, 0,0);
      twistside_cw();
      turn_cw_while_scan(2,0, (int[]){37,33}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(4,0, (int[]){46,35,43,29}, 0,0);
      twistside_cw();
      turn_cw_while_scan(6,0, (int[]){52,53,41,47,46,45}, 0,0);
      twistside_cw();
      turn_cw_while_scan(4,0, (int[]){39,51,52,53}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(2,0, (int[]){41,38}, 0,0);
      twistside_ccw();
      turn_cw_while_scan(4,0, (int[]){5,36,39,42}, 0,0);
      twistside_cw();
      turn_cw_while_scan(8,0, (int[]){3,6,7,8,5,2,1,0}, 0,0);

      // use current colors to do a temporary calibration of first sensor
      for (i=0; i<3; i++)
      {   whitecalibration[0][i] = 0;
      }
      int j;
      for (j=0; j<54; j++)
      {   if (facergb[j][0]+facergb[j][1]+facergb[j][2] > whitecalibration[0][0]+whitecalibration[0][1]+whitecalibration[0][2])
          {   copyrgb(facergb[j],whitecalibration[0]);
          }
      }
      havecalibratedtwosensors = false;
      //                              not scanned: 4,13,22,31,40,49
      calibrate24colors(whitecalibration[0], (int[]){0,1,2,3,5,6,7,8,9,10,11,16,17,18,19,21,24,25,37,42,43,45,48,51});
      calibrate24colors(whitecalibration[0], (int[]){12,14,15,20,23,26,27,28,29,30,32,33,34,35,36,38,39,41,44,46,47,50,52,53});
      return true;
   }
}

void transform_rgb_to_colors()
{
    bool collected[54];
    int i,j;
    for (i=0; i<54; i++)
    {   collected[i] = 0;
        facecolors[i] = COLOR_RED;    // this is default when nothing else is detected
    }
    // prevent center pieces from being collected
    collected[4] = collected[13] = collected[22] = collected[31] = collected[40] = collected[49] = 1;

    // collect the 8 faces with lowest saturation   -> WHITE
    for (i=0; i<8; i++)
    {   int best = 0;
        int bestval = 1000000000;
        for (j=0; j<54; j++)
        {   int val = compute_s(facergb[j]);
            if (!collected[j] && val<bestval)
            {   best = j;
                bestval = val;
            }
        }
        facecolors[best] = COLOR_WHITE;
        collected[best] = 1;
    }
    // collect the 8 faces with hue nearest 240  -> BLUE
    for (i=0; i<8; i++)
    {   int best = 0;
        int bestval = 1000000000;
        for (j=0; j<54; j++)
        {   int val = abs(compute_h(facergb[j])-240);
            if (!collected[j] && val<bestval)
            {   best = j;
                bestval = val;
            }
        }
        facecolors[best] = COLOR_BLUE;
        collected[best] = 1;
    }
    // collect the 8 faces with hue nearest 122  -> GREEN
    for (i=0; i<8; i++)
    {   int best = 0;
        int bestval = 1000000000;
        for (j=0; j<54; j++)
        {   int val = abs(compute_h(facergb[j])-122);
            if (!collected[j] && val<bestval)
            {   best = j;
                bestval = val;
            }
        }
        facecolors[best] = COLOR_GREEN;
        collected[best] = 1;
    }
    // collect the 8 faces with hue nearest 43  -> YELLOW
    for (i=0; i<8; i++)
    {   int best = 0;
        int bestval = 1000000000;
        for (j=0; j<54; j++)
        {   int val = abs(compute_h(facergb[j])-43);
            if (!collected[j] && val<bestval)
            {   best = j;
                bestval = val;
            }
        }
        facecolors[best] = COLOR_YELLOW;
        collected[best] = 1;
    }
    // the remaining faces are probably orange or red
    // since distinction is not easily possible without further info,
    // just deliver both hues as RED
}

void fix_orange_on_corner(int* facelets)
{
    int i;
    for (i=0; i<3; i++)
    {   byte c0 = facecolors[facelets[i]];
        byte c1 = facecolors[facelets[(i+1)%3]];
        byte c2 = facecolors[facelets[(i+2)%3]];
        if (c0==COLOR_RED)
        {   if (   (c1==COLOR_BLUE && c2==COLOR_WHITE)
                || (c1==COLOR_YELLOW && c2==COLOR_BLUE)
                || (c1==COLOR_WHITE && c2==COLOR_GREEN)
                || (c1==COLOR_GREEN && c2==COLOR_YELLOW)
                )
            {   facecolors[facelets[i]] = COLOR_ORANGE;
            }
        }
    }
}

int find_edge_facelet(byte color, byte othercolor)
{
    int i;
    for (i=0; i<12; i++)
    {   int f1 = edgefacelets[i][0];
        int f2 = edgefacelets[i][1];
        if (facecolors[f1]==color && facecolors[f2]==othercolor)
        {   return f1;
        }
        if (facecolors[f2]==color && facecolors[f1]==othercolor)
        {   return f2;
        }
    }
    return -1;
}

void fix_orange_edges(byte othercolor)
{
    // find first occurence
    int f1 = find_edge_facelet(COLOR_RED, othercolor);
    if (f1<0)
    {  return;    // must be wrong scan
    }
    // temporarily change to orange
    facecolors[f1] = COLOR_ORANGE;
    // find second occurence
    int f2 = find_edge_facelet(COLOR_RED, othercolor);
    if (f2<0)
    {   return;   // must be wrong scan
    }
    // check if second face ist more "orange" than first one -> must revert assignment
    if (is_more_orange(facergb[f2], facergb[f1]))
    {   facecolors[f2] = COLOR_ORANGE;
        facecolors[f1] = COLOR_RED;
    }
}

// scan the whole cube
bool scan()
{
    // do the scan run to get rgb values for all facelets
    if (!scan_all_sides())
    {   return false;
    }

    // convert all rgb values to colors
    transform_rgb_to_colors();

    // try to fix up the orange/red confusion
    fix_orange_on_corner((int[]){0,36,47});
    fix_orange_on_corner((int[]){2,45,11});
    fix_orange_on_corner((int[]){8,9,20});
    fix_orange_on_corner((int[]){6,18,38});
    fix_orange_on_corner((int[]){27,44,24});
    fix_orange_on_corner((int[]){29,26,15});
    fix_orange_on_corner((int[]){35,17,51});
    fix_orange_on_corner((int[]){33,53,42});
    fix_orange_edges(COLOR_WHITE);
    fix_orange_edges(COLOR_YELLOW);
    fix_orange_edges(COLOR_GREEN);
    fix_orange_edges(COLOR_BLUE);

    // the center facelets could not be scanned, so use hardcoded assignment
    facecolors[4]  = COLOR_YELLOW;
    facecolors[31] = COLOR_WHITE;
    facecolors[13] = COLOR_RED;
    facecolors[40] = COLOR_ORANGE;
    facecolors[22] = COLOR_BLUE;
    facecolors[49] = COLOR_GREEN;

    return true;
}




