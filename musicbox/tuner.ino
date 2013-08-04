

void tuner_setup()
{
  // build coefs
  for (i=0; i<NUMBINS; i++)
  {
    p = 0.5*pow(2.,(-i/12.));
    f = exp(-p/Q);
    k[i] = 1./p;
    k2[i] = k[i]/100.;
    a[i*2] = 1.-f;
    a[i*2+1] = 1.-f;
    indhelper[i] = 1.0*TRIGTABLESIZE/k[i]/timerinterval;
    /*
    Serial.print(i);
    Serial.print("\t");
    Serial.print(p);
    Serial.print("\t");
    Serial.print(k[i]);
    Serial.print("\t");
    Serial.print(f);
    Serial.print("\t");
    Serial.print(k2[i]);
    Serial.print("\t");
    Serial.println(a[i*2]);
    */
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
  Timer1.begin(timerCallback1, timerinterval); // period in us - 451 ~ C#
  currtime = 0;
}

void tuner_kill ()
{
  Timer1.end();
}

void timerCallback1() {

  // grab the analog value
  currtime = micros();
  //currtime += timerinterval;
  
  f_data = (float)((analogRead(A0)-(2<<(RESOLUTION-2))) / (1.*(2<<(RESOLUTION-3))));
  //f_data = (float)((data-2048.)*gain);
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
  for (i=0; i<12; i++) {
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
    for (i=12; i<30; i++) {
      ind = ((int)(currtime*indhelper[i]))%TRIGTABLESIZE;
      q_trigmap[i*2] = q_sintable[ind];
      q_trigmap[i*2+1] = q_costable[ind];
    }
    arm_scale_q15(q_trigmap+12, q_data1, 0, q_val+12, 18*2);
    arm_sub_q15(q_val+12, q_pow+12, temp+12, 18*2);
    arm_mult_q15(q_a+12, temp+12, temp+12, 18*2);
    arm_add_q15(temp+12, q_pow+12, q_pow+12, 18*2);
  }
  
  // for low-frequency
  if (counter2==1)
  {
    for (i=30; i<48; i++) {
      ind = ((int)(currtime*indhelper[i]))%TRIGTABLESIZE;
      q_trigmap[i*2] = q_sintable[ind];
      q_trigmap[i*2+1] = q_costable[ind];
    }
    arm_scale_q15(q_trigmap+30, q_data2, 0, q_val+30, 18*2);
    arm_sub_q15(q_val+30, q_pow+30, temp+30, 18*2);
    arm_mult_q15(q_a+30, temp+30, temp+30, 18*2);
    arm_add_q15(temp+30, q_pow+30, q_pow+30, 18*2);
  }
    
    // compute a bit of power
    arm_cmplx_mag_q15(q_pow+currbin, q_mag+currbin, 1);
    
    //arm_mult_q15(q_mag+currbin, q_k+currbin, q_mag+currbin, 1);
    currbin += 1;
    if (currbin == NUMBINS) currbin = 0;

    if (counter1==1)
      counter1 = 0;
    else
      counter1 += 1;
      
    if (counter2==3)
      counter2 = 0;
    else
      counter2 += 1;
    endtime = micros();
    //Serial.println(f_data*10);
    //Serial.print("\t");
    //Serial.println(q_data1);
    //Serial.println(endtime-currtime);
}
