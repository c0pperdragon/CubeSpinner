// ------------------ CONTROLLING THE MACHINE ------------------


int current_angle(byte output)
{
    int angle;
    OutputGetCount(output,&angle);
    return angle;
}

int rounded_90(int angle)
{
    if (angle>=0)
    {   return ((angle+45)/90)*90;
    }
    else
    {   return -(((-angle)+45)/90)*90;
    }
}

bool ismotorbusy(byte output)
{
  bool busy;
  if (output & OUT_A)
  { OutputTest(OUT_A, &busy);
    if (busy)
    {   return true;
    }
  }
  if (output & OUT_B)
  { OutputTest(OUT_B, &busy);
    if (busy)
    {   return true;
    }
  }
  if (output & OUT_C)
  { OutputTest(OUT_C, &busy);
    if (busy)
    {   return true;
    }
  }
  if (output & OUT_D)
  { OutputTest(OUT_D, &busy);
    if (busy)
    {   return true;
    }
  }
  return false;
}

void waitwhilebusy(byte output)
{
    while (ismotorbusy(output))
    {   Wait(MS_1);
    }
}

void lock_motors()
{
    OutputStop(OUT_ABCD, 1);
}

void turn_cw()    // cube turns clockwise when seen from the yellow side
{
    waitwhilebusy(OUT_AD);
    int targeta = rounded_90(current_angle(OUT_A))-90;

    OutputPolarity(OUT_AD,-1);
    OutputStepPowerEx(OUT_AD, 100, 0,90, 0, TRUE, OWNER_NONE);

    while (ismotorbusy(OUT_AD) && current_angle(OUT_A)>targeta+30)
    {   Wait(1);
    }
}

void turn_ccw()   // cube turns counter-clockwise when seen from the yellow side
{
    waitwhilebusy(OUT_AD);
    int targeta = rounded_90(current_angle(OUT_A))-270;

    OutputPolarity(OUT_AD,-1);
    OutputStepPowerEx(OUT_AD, 100, 0, 270, 0, TRUE, OWNER_NONE);

    while (ismotorbusy(OUT_AD) && current_angle(OUT_A)>targeta+30)
    {   Wait(1);
    }
}

void turn_180()    // cube turns around on the yellow-white axis
{
    waitwhilebusy(OUT_AD);
    int targeta = rounded_90(current_angle(OUT_A))-180;

    OutputPolarity(OUT_AD,-1);
    OutputStepPowerEx(OUT_AD, 100, 0, 180, 0, TRUE, OWNER_NONE);

    while (ismotorbusy(OUT_AD) && current_angle(OUT_A)>targeta+25)
    {   Wait(1);
    }
}

void twistside_cw()
{
    waitwhilebusy(OUT_C);
    int overshoot = 30;
    OutputPolarity(OUT_C,-1);
    OutputStepPowerEx(OUT_C, 100, 0, 90+overshoot, 0, TRUE, OWNER_NONE);
    waitwhilebusy(OUT_C);
    OutputPolarity(OUT_C,1);
    OutputStepPowerEx(OUT_C, 100, 0, overshoot, 0, TRUE, OWNER_NONE);
}

void twistside_ccw()
{
    waitwhilebusy(OUT_C);
    int overshoot = 30;
    OutputPolarity(OUT_C,1);
    OutputStepPowerEx(OUT_C, 100, 0, 90+overshoot, 0, TRUE, OWNER_NONE);
    waitwhilebusy(OUT_C);
    OutputPolarity(OUT_C,-1);
    OutputStepPowerEx(OUT_C, 100, 0, overshoot, 0, TRUE, OWNER_NONE);
}

void twistside_180()
{
    waitwhilebusy(OUT_C);
    int overshoot = 30;
    OutputPolarity(OUT_C,1);
    OutputStepPowerEx(OUT_C, 100, 0, 180+overshoot, 0, TRUE, OWNER_NONE);
    waitwhilebusy(OUT_C);
    OutputPolarity(OUT_C,-1);
    OutputStepPowerEx(OUT_C, 100, 0, overshoot, 0, TRUE, OWNER_NONE);
}


void twist_updown(int up_quarters_cw, int down_quarters_cw)
{
    int quarters_a = (up_quarters_cw==1)?3:((up_quarters_cw==3?1:up_quarters_cw));
    int quarters_d = down_quarters_cw;
    waitwhilebusy(OUT_AD);

    int targeta = rounded_90(current_angle(OUT_A))+quarters_a*90;
    int targetd = rounded_90(current_angle(OUT_D))+quarters_d*90;

    if (quarters_a)
    {   OutputPolarity(OUT_A,1);
        OutputStepPowerEx(OUT_A, 100, 0, 90*quarters_a, 0, TRUE, OWNER_NONE);
    }
    if (quarters_d)
    {   OutputPolarity(OUT_D,1);
        OutputStepPowerEx(OUT_D, 100, 0, 90*quarters_d, 0, TRUE, OWNER_NONE);
    }

    while ((ismotorbusy(OUT_A) && current_angle(OUT_A)<targeta-25)
        || (ismotorbusy(OUT_D) && current_angle(OUT_D)<targetd-25))
    {   Wait(1);
    }
}



