#include "C:\BricxCC\API\ev3_button.h"
#include "C:\BricxCC\API\ev3_lcd.h"
#include "C:\BricxCC\API\ev3_command.h"
#include "C:\BricxCC\API\ev3_output.h"
#include "C:\BricxCC\API\ev3_timer.h"

#include "sensors.c"
#include "motorcontrol.c"
#include "scanner.c"
#include "twophase.c"


// --- common pipe communication tools ---

int send_string(int fd, char* buffer)
{
    int l = strlen(buffer)+1;
    while (l>0)
    {   int didwrite = write(fd,buffer,l);
        if (didwrite<=0 || didwrite>l)
        {   return 0;
        }
        buffer = buffer+didwrite;
        l = l-didwrite;
    }
    return 1;
}

int receive_string(int fd, char* buffer, int buffersize)
{
    int len=0;
    while (len<buffersize)
    {   int didread = read(fd,buffer+len,1);
        if (didread<=0)
        {   if (errno==EINTR)    // strange signal that killed the blocking read
            {   Wait(1);
                continue;
            }
            return 0;            // read error (or eof)
        }
        else
        {   len+=didread;
            if (buffer[len-1]==0)   // detect end of string
            {   return 1;
            }
        }
    }
    return 0;     // buffersize exceeded
}


// ------------------------------- SOLVER PROCESS -----------------------------

int notifydata_fd_write;
char* notifydata_buffer;
int phase1moveslength;
void notifyphase1finished()
{
    // do not send phase1 solution a second time
    if (phase1moveslength>0)
    {   PlayToneEx(700, 300, 70);    // notification about this problem
        return;
    }
    // memorize length of solution
    phase1moveslength = strlen(notifydata_buffer);
    // if the phase1 solution has moves send them right now
    if (phase1moveslength>0)
    {   send_string(notifydata_fd_write, notifydata_buffer);
    }
}

int solvermain(int fd_read, int fd_write)
{
    send_string(fd_write, "Solver 0.3");
    // create tables
    twophase_init();
    // notify spinner process that all is ready
    send_string(fd_write, "Ready");

    // handle solve requests from spinner
    char buffer[100];
    while (receive_string(fd_read, buffer, 100))
    {   if (strlen(buffer)<54)
        {   send_string(fd_write, "Err 7");
        }
        else
        {   // translate from ASCII to binary representation
            int i;
            for (i=0; i<54; i++)
            {   buffer[i] = buffer[i] - 'A';
            }

            char moves[101];

            // set up data for the callback
            notifydata_fd_write = fd_write;
            notifydata_buffer = moves;
            phase1moveslength = 0;

            int time_start = TimerGetMS();

            // compute solution (callback will send first part)
            solution(buffer,50,50, moves, &notifyphase1finished);

            int time_finish = TimerGetMS();

            // when phase1 had no moves at all, no data was sent then.
            // send empty data packet now and fill solution
            if (phase1moveslength<=0)
            {   send_string(fd_write,"");
                send_string(fd_write,moves);
            }
            // send back second part of solution only
            else
            {
              send_string(fd_write,moves+phase1moveslength);
            }

            // additionaly send total solve time
            char buffer[100];
            sprintf (buffer, "%d", (int) (time_finish-time_start));
            send_string(fd_write, buffer);
        }
    }
}


// -------- ------------------ MAIN PROCESS -----------------------------

// --- translate the movements from side-notation to turns and twists

int currentrotation;

void rotate_to(int r)
{
    if (r!=currentrotation)
    {   switch ((r+4 - currentrotation) % 4)
        { case 1:
            turn_cw();
            break;
          case 2:
            turn_180();
            break;
          case 3:
            turn_ccw();
            break;
        }
        currentrotation = r;
    }
}


int parsetwistlength(char* moves, int* cursor)
{
    switch (moves[*cursor])
    {   case '2':
          (*cursor)++;
          return 2;
        case '\'':
          (*cursor)++;
          return 3;
        default:
          return 1;
    }
}

void dotwists(char* moves)
{
    int cursor=0;
    for (;;)
    {   char what = moves[cursor++];
        if (what==0)
        {   return;
        }
        int twistlength = parsetwistlength(moves,&cursor);
        switch (what)
        { case 'U':
            if (moves[cursor]=='D')   // do a move of both faces
            {   cursor++;
                int how_d = parsetwistlength(moves,&cursor);
                twist_updown(twistlength,how_d);
            }
            else
            {   twist_updown(twistlength,0);
            }
            break;

          case 'D':
            if (moves[cursor]=='U')   // move both faces
            {   cursor++;
                int how_u = parsetwistlength(moves,&cursor);
                twist_updown(how_u,twistlength);
            }
            else
            {  twist_updown(0,twistlength);
            }
            break;

          default:
            rotate_to((what=='B')?0:(what=='L')?1:(what=='F')?2:3);
            if (twistlength==2)
            {  twistside_180();
            }
            else if (twistlength==3)
            {   twistside_ccw();
            }
            else
            {   twistside_cw();
            }
            break;
        }
    }
}

// check scan result for correctnes, but do not try to solve yet
//   0: ok
//   1: invalid facelet counts
//   2: Not all 12 edges exist exactly once
//   3: Flip error: One edge has to be flipped
//   4: Not all corners exist exactly once
//   5: Twist error: One corner has to be twisted
//   6: Parity error: Two corners or two edges have to be exchanged

int check_scan()
{
    struct _FaceCube _fc;
    FaceCube fc = FaceCube_construct(&_fc, facecolors);
    if (FaceCube_verifyCounts(fc)!=0)
    {   return 1;
    }
    struct _CubieCube _cc;
   	CubieCube cc = CubieCube_constructFromFaceCube(&_cc,fc);
    return CubieCube_verify(cc);
}


// ---------------------------- error situation logging ---------------

void logrgbvalues()
{
  FILE* f = fopen("/tmp/rgblog.txt", "wb");

  int i;
  for (i=0; i<54; i++)
  {   fprintf (f, "%d: %c %d %d %d\n",i, colorletters[facecolors[i]], facergb[i][0], facergb[i][1], facergb[i][2]);
  }

  fclose(f);
}


// ----------------- CLEANUP CODE -------------------

void cleanup()
{
  ButtonLedExit();
  SensorExit();
  OutputExit();
  SoundExit();
  LcdExit();
  exit(0);
}

// ----------- main route for scanning / manipulating of cube --------------
int spinnermain(int fd_read, int fd_write)
{
  char buffer[100];
  int i;

  // startup code
  LcdInit();
  SoundInit();
  if (!OutputInit())
  {  cleanup();
  }
  if (!SensorInit())
  { cleanup();
  }
  if(!ButtonLedInit())
  { cleanup();
  }

  // lock the motors to current position
  // (so inserting the cube does not de-calibrate the levers)
  lock_motors();

  // wait until the solver process has initialized its tables
  SensorSetUARTOperatingModes(COL_REFLECT,0,0,COL_REFLECT);

  LcdClean();
  LcdText(1, 0,0, "Creating tables...");

  for (i=0; i<7; i++)
  {   int readok = receive_string(fd_read, buffer, 100);
      if (!readok)
      {   LcdText(1,0,16+i*16,"No reply");
          Wait(500);
          cleanup();
      }
      LcdText(1,0,16+i*16, buffer);
      if (buffer[0]=='R')   // detect "Ready" message
      {  break;
      }
  }

  for (;;)
  {   // prompt to receive cube or close program
      SensorSetUARTOperatingModes(COL_RGBRAW, 0,0, COL_RGBRAW);

      unsigned short but = ButtonWaitForAnyPress(0);
      if (but == BUTTON_ID_ESCAPE)
      {   cleanup();
      }

      LcdClean();
      LcdText(1, 0,32, "Scanning...");
      int time_start = TimerGetMS();

      // perform the scan
      if (!scan())
      {   LcdClean();
          LcdText (1, 0,10, "Calibration finished");
          continue;
      }

      // do a preliminary check for correctness and try to apply a fix for
      // the red/orange edge detection problem
      int chk = check_scan();

      // in case of scann error, give error beep and write debug info
      if (chk!=0)
      {   PlayToneEx(300, 400, 70);         // failure sound

          LcdClean();
          sprintf (buffer, "Err %d",chk);
          LcdText (1, 90,10, buffer);
          print_side(44,  0,  0);         // U
          print_side(88, 43,  9);         // R
          print_side(44, 43,  18);        // F
          print_side(44, 86,  27);        // D
          print_side(2,  43,  36);        // L
          print_side(132,43,  45);        // B

          logrgbvalues();
          continue;
      }

      SensorSetUARTOperatingModes(COL_REFLECT,0,0,COL_REFLECT);
      int time_startsolve1 = TimerGetMS();

      // send the problem to the solver process
      char problem[55];
      for (i=0; i<54; i++)
      {   problem[i] = 'A' + facecolors[i];
      }
      problem[54] = 0;
      send_string(fd_write, problem);


      // receive first part of solution (blocking operation)
      char moves1[101];
      int readok = receive_string(fd_read, moves1, 101);

      // check unexpected error condition (should not happen because of previous checks)
      if (moves1[0]=='E' || !readok)
      {    LcdClean();
           LcdText (1, 10,10, moves1);
           Wait(500);
           cleanup();
      }

      int time_starttwist1 = TimerGetMS();

      // because the scan process has left the cube while still spinning, must
      // wait in case the solution was found quicker than the cube took to
      // come to rest
      waitwhilebusy(OUT_ABCD);

      // perform first part of solution
      currentrotation = 0;
      dotwists(moves1);

      int time_startsolve2 = TimerGetMS();

      // receive second part of solution
      char moves2[101];
      readok = receive_string(fd_read, moves2, 101);

      // check unexpected error condition (should not happen because of previous checks)
      if (moves1[0]=='E' || !readok)
      {    LcdClean();
           LcdText (1, 10,10, moves1);
           Wait(500);
           cleanup();
      }

      int time_starttwist2 = TimerGetMS();

      // perform second part of solution
      dotwists(moves2);

      long time_end = TimerGetMS();

      // read the time measurement from the solver process
      char solvetime[20];
      receive_string(fd_read, solvetime, 20);

      // write solution to screen (for debug purposes)
      LcdClean();
      strcpy (moves1+strlen(moves1), moves2);   // copy parts together in single buffer
      int len1 = strlen(moves1);
      int movecount = 0;
      for (i=0; i<len1; i+=16)
      {   int j;
          for (j=0; j<16 && i+j<len1; j++)
          {   buffer[j] = moves1[i+j];
              if (moves1[i+j]!='2' && moves1[i+j]!='\'')
              {  movecount++;
              }
          }
          buffer[j]=0;
          if (i+j==len1)
          {   sprintf (buffer+j, " (%d)", movecount);
          }
          LcdText(1, 0,(i/16)*16, buffer);
      }

      // write time measurements summary
      int ms = (int)  (time_startsolve1-time_start);
      int frac = ms%1000;
      sprintf (buffer, "scan:  %d.%s%d", ms/1000, frac<10?"00":frac<100?"0":"", frac);
      LcdText(1, 8,4*16, buffer);

      ms = (int) ((time_starttwist1-time_startsolve1)+(time_starttwist2-time_startsolve2));
      frac = ms%1000;
      sprintf (buffer, "solve: %d.%s%d (%s)", ms/1000, frac<10?"00":frac<100?"0":"", frac, solvetime);
      LcdText(1, 8,5*16, buffer);

      ms = (int) ((time_startsolve2-time_starttwist1)+(time_end-time_starttwist2));
      frac = ms%1000;
      sprintf (buffer, "twist: %d.%s%d", ms/1000, frac<10?"00":frac<100?"0":"", frac);
      LcdText(1, 8,6*16, buffer);

      ms = (int)  (time_end-time_start);
      frac = ms%1000;
      sprintf (buffer, "total: %d.%s%d", ms/1000, frac<10?"00":frac<100?"0":"", frac);
      LcdText(1, 8,7*16, buffer);
   }
}


// ------------------- PROCESS LAUNCHER ---------------------------

int main()
{
    // create pipes for communication between processes
    int pipefd_p2c[2];
    int pipefd_c2p[2];
    if (pipe(pipefd_p2c)<0)
    {   return;
    }
    if (pipe(pipefd_c2p)<0)
    {   return;
    }

    // fork off the child process (the solver)
    int pid = fork();
    if (pid<0)   // failure, no child was created
    {   return;
    }
    else if (pid==0) // I am the child
    {   close(pipefd_p2c[1]);    // Close unused pipe ends
        close(pipefd_c2p[0]);
        solvermain(pipefd_p2c[0],pipefd_c2p[1]);
    }
    else            // I am the parent
    {   close(pipefd_p2c[0]);    // Close unused pipe ends
        close(pipefd_c2p[1]);
        spinnermain(pipefd_c2p[0],pipefd_p2c[1]);
    }
    return 0;
}


