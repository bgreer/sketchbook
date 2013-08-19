// party mode
float posoffset = 0.0;
uint8_t amplitude[6];
float pre_amp[6], gauss[6];

static uint8_t gaussian[32] = {
     255,
     253,
     249,
     243,
     235,
     224,
     212,
     198,
     183,
     168,
     153,
     137,
     122,
     107,
      93,
      80,
      69,
      58,
      48,
      40,
      33,
      26,
      21,
      17,
      13,
      10,
       8,
       6,
       4,
       3,
       2,
       0
};

static float cfr[6] = {
  1.0, 
  1.0, 
  0.0, 
  0.0, 
  0.0, 
  1.0
};
static float cfg[6] = {
  0.0, 
  1.0, 
  1.0, 
  1.0, 
  0.0, 
  0.0
};
static float cfb[6] = {
  0.0, 
  0.0, 
  0.0, 
  1.0, 
  1.0, 
  1.0
};


void party_colors()
{
  posoffset += 0.01;
  if (posoffset >= 12.) posoffset -= 12.;
  for (i=0; i<6; i++)
  {
    pre_amp[i] = constrain(0.95*pre_amp[i] + q_mag[i]*k[i]*0.1, 50.0, 255.0);
    amplitude[i] = (uint8_t)pre_amp[i];
  }
  
  for (i=0; i<12; i++)
    r_b[i] = g_b[i] = b_b[i] = 0;
    
  for (j=0; j<6; j++)
  {
    for (i=0; i<12; i++)
    {
      x = i - posoffset;
      if (x < 0.) x += 12.;
      x = abs(x - 2.*j);
      if (x > 6.0) x = 12. - x;
      r_b[i] += cfr[j]*(amplitude[j]/255.)*gaussian[(uint8_t)min(x*1500/amplitude[j],30)];
      g_b[i] += cfg[j]*(amplitude[j]/255.)*gaussian[(uint8_t)min(x*1500/amplitude[j],30)];
      b_b[i] += cfb[j]*(amplitude[j]/255.)*gaussian[(uint8_t)min(x*1500/amplitude[j],30)];
    }
  }
  for (i=0; i<12; i++)
  {
    r[i] = constrain(r_b[i], 0, 255);
    g[i] = constrain(g_b[i], 0, 255);
    b[i] = constrain(b_b[i], 0, 255);
  }
}

void party_setup()
{
  // build coefs
  for (i=0; i<6; i++)
  {
    p = 0.3*pow(2.5,-i);
    f = exp(-p/10.);
    k[i] = 1./p;
    k2[i] = k[i]/100.;
    a[i*2] = 1.-f;
    a[i*2+1] = 1.-f;
    indhelper[i] = 1.0*TRIGTABLESIZE/k[i]/timerinterval;
  }
  arm_float_to_q15(a, q_a, NUMBINS*2);
  arm_float_to_q15(k2, q_k, NUMBINS);

  // build trig tables
  for (i=0; i<TRIGTABLESIZE; i++)
  {
    costable[i] = cos(i*2.*3.141592/TRIGTABLESIZE);
    sintable[i] = sin(i*2.*3.141592/TRIGTABLESIZE);
  }
  arm_float_to_q15(costable, q_costable, TRIGTABLESIZE);
  arm_float_to_q15(sintable, q_sintable, TRIGTABLESIZE);

  counter1 = 0;
  counter2 = 1;
  
  currbin = 0;
  Timer1.begin(partycallback, 170);
  currtime = 0;
}

void party_kill ()
{
  Timer1.end();
}

void partycallback() {

  // grab the analog value
  currtime = micros();
  //currtime += timerinterval;
  
  f_data = (float)((analogRead(A0)-(2<<(RESOLUTION-2))) / (1.*(2<<(RESOLUTION-2))));
  //f_data = (float)analogRead(0);
  arm_float_to_q15(&f_data, &q_data, 1);
  
  if (counter1==0)
    f_data1 = f_data;
  else if (counter1==1)
  {
    f_data1 += f_data;
    f_data1 *= 0.5;
    arm_float_to_q15(&f_data1, &q_data1, 1);
  }
  
  if (counter2==0)
    f_data2 = f_data;
  else if (counter2==1 || counter2==2)
    f_data2 += f_data;
  else if (counter2==3)
  {
    f_data2 += f_data;
    f_data2 *= 0.25;
    arm_float_to_q15(&f_data2, &q_data2, 1);
  }
  
  // for high frequency stuff, every cycle
  for (i=0; i<6; i++) {
    ind = ((int)(currtime*indhelper[i]))%TRIGTABLESIZE;
    q_trigmap[i*2] = q_sintable[ind];
    q_trigmap[i*2+1] = q_costable[ind];
  }
  arm_scale_q15(q_trigmap, q_data, 0, q_val, 12*2);
  arm_sub_q15(q_val, q_pow, temp, 12*2);
  arm_mult_q15(q_a, temp, temp, 12*2);
  arm_add_q15(temp, q_pow, q_pow, 12*2);
  
  // for mid-frequency
  if (counter1==1)
  {
    for (i=6; i<6; i++) {
      ind = ((int)(currtime*indhelper[i]))%TRIGTABLESIZE;
      q_trigmap[i*2] = q_sintable[ind];
      q_trigmap[i*2+1] = q_costable[ind];
    }
    arm_scale_q15(q_trigmap+12, q_data1, 0, q_val+12, 18*2);
    arm_sub_q15(q_val+12, q_pow+12, temp+12, 18*2);
    arm_mult_q15(q_a+12, temp+12, temp+12, 18*2);
    arm_add_q15(temp+12, q_pow+12, q_pow+12, 18*2);
  }

    // compute a bit of power
    arm_cmplx_mag_q15(q_pow+currbin, q_mag+currbin, 1);
    
    //arm_mult_q15(q_mag+currbin, q_k+currbin, q_mag+currbin, 1);
    currbin += 1;
    if (currbin == 6) currbin = 0;

    if (counter1==1)
      counter1 = 0;
    else
      counter1 += 1;
      
    if (counter2==3)
      counter2 = 0;
    else
      counter2 += 1;
    //endtime = micros();
    //Serial.println(f_data);
    //Serial.print("\t");
    //Serial.println(q_data1);
    //Serial.println(endtime-currtime);
}
