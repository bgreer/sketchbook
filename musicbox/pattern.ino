float pos[3];
float dir[3];
float colval[3];
int mult[3];
uint8_t ij;

void pattern_destroy()
{
  Timer1.end();
}

void pattern_setup()
{
  pos[0] = 0.0;
  pos[1] = 3.0;
  pos[2] = 5.0;
  
  colval[0] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[0] = 0.055;
  mult[0] = random(4);
  
  colval[1] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[1] = 0.05;
  mult[1] = random(4);
  
  colval[2] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[2] = 0.06;
  mult[2] = random(4);
  
  //Timer1.begin(patterncallback, 10000);
}



void patterncallback()
{
  pos[0] += 0.003;
  pos[1] += 0.002;
  pos[2] -= 0.001;
  
  //f_data = (float)((analogRead(A0)-(2<<(RESOLUTION-2))) / (1.*(2<<(RESOLUTION-6))));
  //Serial.println(f_data);
  
  colval[0] += dir[0];
  if (colval[0] > 255) {colval[0] = 255; dir[0] = -dir[0];}
  if (colval[0] < PATTERN_MIN) {colval[0] = PATTERN_MIN; dir[0] = -dir[0]; mult[0] = random(3)+1;}
  colval[1] += dir[1];
  if (colval[1] > 255) {colval[1] = 255; dir[1] = -dir[1];}
  if (colval[1] < PATTERN_MIN) {colval[1] = PATTERN_MIN; dir[1] = -dir[1]; mult[1] = random(3)+1;}
  colval[2] += dir[2];
  if (colval[2] > 255) {colval[2] = 255; dir[2] = -dir[2];}
  if (colval[2] < PATTERN_MIN) {colval[2] = PATTERN_MIN; dir[2] = -dir[2]; mult[2] = random(3)+1;}
  //currtime = micros();
  for (ij=0; ij<12; ij++)
  {
    r[ij] = (uint8_t)((int)(colval[0]*pow(sin(mult[0]*PI*(ij-pos[0])/12.),2.)));
    g[ij] = (uint8_t)((int)(colval[1]*pow(sin(mult[1]*PI*(ij-pos[1])/12.),2.)));
    b[ij] = (uint8_t)((int)(colval[2]*pow(sin(mult[2]*PI*(ij-pos[2])/12.),2.)));
  }
  //endtime = micros();
  //Serial.println(endtime-currtime);
}
