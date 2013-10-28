// this is for the guy helmet

float pos[3];
float dir[3];
float colval[3];
int mult[3];

void pattern_init()
{
	pos[0] = 0.0;
  pos[1] = 3.0;
  pos[2] = 5.0;
  
  colval[0] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[0] = 0.21;
  mult[0] = random(3)+1;
	mult[0] = 0;
  
  colval[1] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[1] = 0.2;
  mult[1] = random(3)+1;
	mult[1] =0 ;
  
  colval[2] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[2] = 0.3;
  mult[2] = random(3)+1;

}

void pattern_step()
{
  pos[0] += 0.07;
  pos[1] += 0.05;
  pos[2] -= 0.03;

  colval[0] += dir[0];
  if (colval[0] > 255) {colval[0] = 255; dir[0] = -dir[0];}
  if (colval[0] < PATTERN_MIN) {colval[0] = PATTERN_MIN; dir[0] = -dir[0]; mult[0] = random(4)+1;}
  colval[1] += dir[1];
  if (colval[1] > 255) {colval[1] = 255; dir[1] = -dir[1];}
  if (colval[1] < PATTERN_MIN) {colval[1] = PATTERN_MIN; dir[1] = -dir[1]; mult[1] = random(4)+1;}
  colval[2] += dir[2];
  if (colval[2] > 255) {colval[2] = 255; dir[2] = -dir[2];}
  if (colval[2] < PATTERN_MIN) {colval[2] = PATTERN_MIN; dir[2] = -dir[2]; mult[2] = random(4)+1;}
  //currtime = micros();
  for (ij=0; ij<8; ij++)
  {
    rr[ij] = (uint8_t)((int)(colval[0]*pow(sin(mult[0]*PI*(ij-pos[0])/16.),2.)));
    rg[ij] = (uint8_t)((int)(colval[1]*pow(sin(mult[1]*PI*(ij-pos[1])/16.),2.)));
    rb[ij] = (uint8_t)((int)(colval[2]*pow(sin(mult[2]*PI*(ij-pos[2])/16.),2.)));

		lr[ij] = (uint8_t)((int)(colval[0]*pow(sin(mult[0]*PI*(8-ij-pos[0])/16.),2.)));
    lg[ij] = (uint8_t)((int)(colval[1]*pow(sin(mult[1]*PI*(8-ij-pos[1])/16.),2.)));
    lb[ij] = (uint8_t)((int)(colval[2]*pow(sin(mult[2]*PI*(8-ij-pos[2])/16.),2.)));
  }
	mapcolors();

}
