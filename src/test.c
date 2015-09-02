#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "C:\BricxCC\API\ev3_lcd.h"
#include "C:\BricxCC\API\ev3_command.h"
#include "C:\BricxCC\API\ev3_output.h"
#include "C:\BricxCC\API\ev3_sound.h"
#include "C:\BricxCC\API\ev3_timer.h"

#include "sensors.c"
#include "motorcontrol.c"
#include "scanner.c"
//#include "twophase.c"


// --------------------------- TEST CASES ----------------------


singlemovetimings()
{
    long start = TimerGetMS();

    twistside_cw();
    turn_180();
    twistside_cw();
    turn_180();
    twistside_cw();
    turn_180();
    twistside_cw();
    turn_180();
    twistside_cw();
    turn_180();
    twistside_cw();
    turn_180();

    long end = TimerGetMS();

    byte buffer[100];
    sprintf (buffer, "Total: %d", (int)(end-start));
    LcdText(1, 0,32, buffer);
}

/*
void  simulatescan()
{
    long start = TimerGetMS();

    int i;
    for (i=0; i<2; i++)
    {     turn_cw();
          twistside_180();
          twistwhite_cw();
          twistside_ccw();
          turn_cw();
          twistside_ccw();
          turn_180();
          twistside_ccw();
          turn_ccw();
          twistyellow_ccw();
          twistside_cw();
          turn_180();
          twistside_cw();
          turn_cw();
          twistwhite_180();
          twistside_180();
    }

    long end = TimerGetMS();

    byte buffer[100];
    sprintf (buffer, "Total: %d", (int)(end-start));
    LcdText(1, 0,32, buffer);
}
*/

void testcolorsensordiodes()
{
//  SensorSetUARTOperatingMode(0, COL_REFLECT);
//  SensorSetUARTOperatingMode(0, COL_AMBIENT);
  SensorSetUARTOperatingModes(COL_RGBRAW, COL_RGBRAW);
//  SensorSetUARTOperatingMode(0, COL_COLOR);
//  SensorSetUARTOperatingMode(1, COL_COLOR);
//  SensorSetUARTOperatingMode(0, COL_REFRAW);
//  SensorSetUARTOperatingMode(0, COL_RGBRAW);
//  SensorSetUARTOperatingMode(0, COL_CAL);
  Wait(2000);
}

void test_nxt_colorsensordiodes()
{
    SensorPortMode(3,MODE_COLORFULL);
    Wait(1000);
    SensorPortMode(3,MODE_COLORRED);
    Wait(1000);
    SensorPortMode(3,MODE_COLORGREEN);
    Wait(1000);
    SensorPortMode(3,MODE_COLORBLUE);
    Wait(1000);
    SensorPortMode(3,MODE_COLORNONE);
    Wait(1000);
}

void testcolorsensortransmission()
{
    SensorSetUARTOperatingMode(0, COL_RGBRAW);
    Wait(1000);

    long start = TimerGetMS();
    int prevpos = SensorUARTGetActual(0);

    int poshistory[1000];
    int timehistory[1000];
    int historylength=0;;

    while (historylength<1000)
    {  long now = TimerGetMS();
        if (now>start+1000) break;

      int pos = SensorUARTGetActual(0);
      if (pos!=prevpos)
      {   poshistory[historylength] = pos;
          timehistory[historylength] = (now-start);
          historylength++;
          prevpos = pos;
      }
    }

    char buffer[100];
    int i;
    for (i=0; i<8 && i<historylength; i++)
    {   sprintf (buffer, "%dms: %d", timehistory[i], poshistory[i]);
        LcdText(1, 10,16*i, buffer);
    }
}

void testcolorreadings()
{
    short raw[3];
    int rgb[3];
    char buffer[100];

    SensorSetUARTOperatingMode(0, COL_RGBRAW);
    SensorUARTDiscardInputBuffer(0);

    for (;;)
    {   int didread = SensorUARTGetShorts(0, raw, 3);
        if (!didread)
        {   Wait(1);
            continue;
        }

        rgb[0] = raw[0];
        rgb[1] = raw[1]*105 / 100;
        rgb[2] = raw[2]*170 / 100;

        LcdClean();

        sprintf (buffer, " %d %d %d", rgb[0], rgb[1], rgb[2]);
        LcdText(1, 16,64, buffer);

        Wait(1000);
    }
}

void testcolorsensoralignment()
{
//   SensorSetUARTOperatingMode(0, COL_RGBRAW);
   SensorSetUARTOperatingModes(COL_RGBRAW,COL_RGBRAW);
Wait(500);
//   turn_cw_while_scan(6,(int[]){0,1,2,3,4,5,6,7},1);
   turn_cw_while_scan(6,(int[]){0,1,2,3,4,5,6,7},(int[]){0,1,2,3,4,5,6,7},0x10);
}

void testcolorsensorresults()
{
   SensorSetUARTOperatingModes(COL_RGBRAW,COL_RGBRAW);
   turn_cw_while_scan(8,0,(int[]){0,1,2,3,4,5,6,7},0);
//   turn_cw_while_scan(8,(int[]){0,1,2,3,4,5,6,7},0,0);

    char buffer[100];
    int i;
    for (i=0; i<8; i++)
    {   sprintf (buffer, "%d %d %d  %d %d %d",
           facergb[i][0], facergb[i][1], facergb[i][2], compute_h(facergb[i]), compute_s(facergb[i]), compute_v(facergb[i]));
        LcdText(1, 0,14*i, buffer);
    }
}

void testmotorpositiondetection()
{
    OutputPolarity(OUT_AD,-1);
    OutputStepPowerEx(OUT_AD, 100, 0, 360, 0, TRUE, OWNER_NONE);

    Wait(100);

    long starttime = TimerGetMS();
    int startpos;
    long prevtime = starttime;
    OutputGetCount(OUT_A,&startpos);
    int prevpos = startpos;

    int poshistory[1000];
    int timehistory[1000];
    int historylength=0;;

    while (historylength<1000)
    {  long time = TimerGetMS();
       if (time>starttime+1000) break;

      int pos;
      OutputGetCount(OUT_A,&pos);

      if (pos!=prevpos || time!=prevtime)
      {   poshistory[historylength] = (pos-startpos);
          timehistory[historylength] = (time-starttime);
          historylength++;
          prevpos = pos;
          prevtime = time;
      }
      Wait(1);
    }

    int i;
    for (i=0; i<historylength && i<176; i++)
    {   DisplaySetPixel(i, 128-timehistory[i]);
        DisplaySetPixel(i, -poshistory[i]);
    }

//    char buffer[100];
//    int i;
//    for (i=0; i<8 && i<historylength; i++)
//    {   sprintf (buffer, "%dms: %d", timehistory[i], poshistory[i]);
//        LcdText(1, 10,16*i, buffer);
//    }
}

void testscan()
{
   SensorSetUARTOperatingMode(0, COL_AMBIENT);
   Wait(100);

   SensorSetUARTOperatingMode(0, COL_RGBRAW);

    scan();

   SensorSetUARTOperatingMode(0, COL_AMBIENT);

    // debug output possibilities
    print_side(44,  0,  0);         // U
    print_side(88, 43,  9);         // R
    print_side(44, 43,  18);        // F
    print_side(44, 86,  27);        // D
    print_side(2,  43,  36);        // L
    print_side(132,43,  45);        // B
}

void theoreticalminscantime()
{
    long start = TimerGetMS();
                             turn_cw();
    twistside_cw();          turn_cw();
    twistside_ccw();         turn_180();
    twistside_cw();          turn_cw();
    twistside_cw();          turn_180();
    twistside_ccw();         turn_cw();
    twistside_ccw();         turn_180();
    twistside_cw();          turn_cw();
    twistside_cw();          turn_ccw();

    long end = TimerGetMS();

    byte buffer[100];
    sprintf (buffer, "Total: %d", (int)(end-start));
    LcdText(1, 0,32, buffer);
}

testrotationsequence()
{
  int i;

  long start = TimerGetMS();

  twist_updown(1,0);
  turn_cw();

  twistside_cw();

  twist_updown(2,0);
  turn_ccw();

  twistside_ccw();

  twist_updown(3,0);

  twistside_180();

  twist_updown(0,1);
  turn_cw();

  twistside_cw();

  twist_updown(1,1);
  turn_180();

  twistside_ccw();

  twist_updown(2,1);
  turn_180();

  twistside_180();

  twist_updown(3,1);

  twistside_180();

  twist_updown(0,2);
  turn_ccw();

  twistside_cw();

  twist_updown(1,2);
  turn_cw();

  twistside_ccw();

  twist_updown(2,2);

  twistside_180();

  twist_updown(3,2);
  turn_180();

  twistside_180();

  twist_updown(0,3);
  turn_cw();

  twistside_cw();

  twist_updown(1,3);
  turn_ccw();

  twistside_ccw();

  twist_updown(2,3);

  twistside_180();

  twist_updown(3,3);
  turn_cw();

  twistside_180();

  long end = TimerGetMS();


  char buffer[100];
  sprintf(buffer, "%d ms",(int)(end-start));
   LcdText(1, 0,16, buffer);
}

/*
long intermediate_time;
void memorize_intermediate_time()
{
  	intermediate_time = TimerGetMS();
}

void onesolve(char* pattern, int line)
{  long start = TimerGetMS();

   char buffer[200];
   solution(pattern ,50,50, buffer, &memorize_intermediate_time);

   long end = TimerGetMS();

   sprintf (buffer, "Done in %d (%d)", (int)((end-start)), (int)(intermediate_time-start));
   LcdText(1, 0,16*line, buffer);
}

void testtwophase()
{
   SensorSetUARTOperatingMode(0, COL_AMBIENT);
   LcdClean();
   LcdText(1, 0,32, "Creating tables...");

  init_and_load_twophase("/tmp/pruningtables.bin");

   LcdClean();
   SensorSetUARTOperatingMode(0, COL_REFLECT);

	onesolve("LLRLUBFRLULUURFLBBRUFUFLRDUUFBBDRBRDFFDULFRDBFDDRBBLDD", 0);
	onesolve("RRRUUDLDLBLFFRURBFDBURFLRRUBDFLDRUFLULBDLULBDDFBBBFDUF", 1);
	onesolve("BUFUURFRFDFDBRFRLDLBLDFDUBBLLURDFULLDLUDLFFUBRBRUBRBDR", 2);
	onesolve("UUUBUFDUBLLLDRRUDRLRDDFFBRRUFBBDRDUFRUFDLLBLLFLFBBBDFR", 3);

   SensorSetUARTOperatingMode(0, COL_AMBIENT);
}
*/

void testbeep()
{
    PlayToneEx(300, 400, 70);
}


// ----------------- LAUNCHER AND CLEANUP CODE -------------------

void cleanup()
{
  SensorExit();
  OutputExit();
  SoundExit();
  LcdExit();
  exit(0);
}



int main()
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
  LcdClean();

//  singlemovetimings();
//  theoreticalminscantime();
// testmotorpositiondetection();
//  simulatesolve();
//    testscan();
//    testcolorsensortransmission();
    testcolorsensorresults();
//    testcolorsensoralignment();
//  testcolorreadings();
//   testtwophase();
//   testcolorsensordiodes();
//    test_nxt_colorsensordiodes();
//   testrotationsequence();
//    testbeep();

   Wait(1000);
   cleanup();
}




