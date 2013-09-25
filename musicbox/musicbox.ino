#define ARM_MATH_CM4
#include <arm_math.h>
#define TRIGTABLESIZE 256
#define NUMBINS 48
#define Q (10.0)
#define RESOLUTION 12

#define VPRG 2
#define SIN 3
#define SCLK 4
#define XLAT 5
#define BLANK 6
#define DCPRG 9
#define GSCLK 10
#define SOUT 22
#define BUTTON 9

#define PATTERN_MIN 10.0

static unsigned char exp_map[256]={
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,3,
  3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,5,5,5,
  5,5,5,5,5,6,6,6,6,6,6,6,7,7,7,7,7,7,8,8,8,8,8,8,9,9,
  9,9,10,10,10,10,10,11,11,11,11,12,12,12,13,13,13,13,
  14,14,14,15,15,15,16,16,16,17,17,18,18,18,19,19,20,
  20,20,21,21,22,22,23,23,24,24,25,26,26,27,27,28,29,
  29,30,31,31,32,33,33,34,35,36,36,37,38,39,40,41,42,
  42,43,44,45,46,47,48,50,51,52,53,54,55,57,58,59,60,
  62,63,64,66,67,69,70,72,74,75,77,79,80,82,84,86,88,
  90,91,94,96,98,100,102,104,107,109,111,114,116,119,
  122,124,127,130,133,136,139,142,145,148,151,155,158,
  161,165,169,172,176,180,184,188,192,196,201,205,210,
  214,219,224,229,234,239,244,250,255
};

static uint16_t exp_map2[256]={
       0,
       0,
       0,
       0,
       1,
       1,
       1,
       1,
       2,
       2,
       2,
       3,
       3,
       3,
       4,
       4,
       4,
       5,
       5,
       5,
       6,
       6,
       7,
       7,
       7,
       8,
       8,
       9,
       9,
      10,
      10,
      11,
      11,
      12,
      12,
      13,
      13,
      14,
      14,
      15,
      16,
      16,
      17,
      18,
      18,
      19,
      20,
      20,
      21,
      22,
      23,
      24,
      24,
      25,
      26,
      27,
      28,
      29,
      30,
      31,
      32,
      33,
      34,
      35,
      36,
      37,
      38,
      39,
      40,
      42,
      43,
      44,
      45,
      47,
      48,
      50,
      51,
      52,
      54,
      55,
      57,
      59,
      60,
      62,
      64,
      65,
      67,
      69,
      71,
      73,
      75,
      77,
      79,
      81,
      83,
      86,
      88,
      90,
      93,
      95,
      98,
     100,
     103,
     106,
     108,
     111,
     114,
     117,
     120,
     123,
     126,
     130,
     133,
     136,
     140,
     143,
     147,
     151,
     155,
     159,
     163,
     167,
     171,
     175,
     180,
     184,
     189,
     194,
     198,
     203,
     208,
     214,
     219,
     224,
     230,
     236,
     242,
     248,
     254,
     260,
     266,
     273,
     280,
     287,
     294,
     301,
     308,
     316,
     324,
     331,
     340,
     348,
     356,
     365,
     374,
     383,
     393,
     402,
     412,
     422,
     432,
     443,
     453,
     464,
     476,
     487,
     499,
     511,
     524,
     536,
     549,
     562,
     576,
     590,
     604,
     619,
     634,
     649,
     665,
     681,
     697,
     714,
     731,
     749,
     767,
     785,
     804,
     823,
     843,
     863,
     884,
     905,
     927,
     949,
     972,
     995,
    1019,
    1044,
    1069,
    1094,
    1121,
    1147,
    1175,
    1203,
    1232,
    1261,
    1291,
    1322,
    1354,
    1386,
    1419,
    1453,
    1488,
    1524,
    1560,
    1597,
    1635,
    1674,
    1714,
    1755,
    1797,
    1840,
    1884,
    1929,
    1975,
    2022,
    2070,
    2119,
    2170,
    2222,
    2275,
    2329,
    2384,
    2441,
    2499,
    2559,
    2620,
    2682,
    2746,
    2811,
    2878,
    2947,
    3017,
    3089,
    3162,
    3238,
    3315,
    3393,
    3474,
    3557,
    3641,
    3728,
    3817,
    3907,
    4000,
    4095
};

float power[NUMBINS], a[NUMBINS*2], k[NUMBINS], k2[NUMBINS];
long t, starttime, endtime;
float costable[TRIGTABLESIZE], sintable[TRIGTABLESIZE];
float indhelper[NUMBINS];
int i, j, data, ind;
float p, f, f_data, f_data1, f_data2;
volatile int ledval;
float gain = 0.003;
long timerinterval = 350;
float trig1, trig2, trig3, ftemp, x;
float colormap[NUMBINS*3];

uint32_t currtime, currbin, counter1, counter2;
uint32_t buttontimer, buttontimer2;
q15_t q_data, q_data1, q_data2;
q15_t q_costable[TRIGTABLESIZE], q_sintable[TRIGTABLESIZE];
q15_t q_trigmap[NUMBINS*2];
q15_t q_val[NUMBINS*2], q_pow[NUMBINS*2];
q15_t temp[NUMBINS*2];
q15_t q_a[NUMBINS*2], q_k[NUMBINS];
q15_t q_mag[NUMBINS];

uint8_t r[12], g[12], b[12];
int r_b[12], g_b[12], b_b[12];
uint8_t *ptr[48];
uint16_t val;
uint8_t val2;
uint8_t needPulse = 0;
IntervalTimer Timer1;
/* these timers are specific to teensy3, use timer registers for regular arduino */
IntervalTimer Timer3;
float buttonval, buttonder, buttonlast;

uint8_t currmode, prevmode;

void setup()
{
  Serial.begin(115200);
  pinMode(VPRG, OUTPUT);
  pinMode(SIN, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(XLAT, OUTPUT);
  pinMode(BLANK, OUTPUT);
  pinMode(DCPRG, OUTPUT);
  pinMode(GSCLK, OUTPUT);
  pinMode(SOUT, INPUT);
  pinMode(13, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  
  digitalWrite(BLANK, LOW);
  digitalWrite(XLAT, LOW);
  digitalWrite(SCLK, LOW);
  digitalWrite(DCPRG, HIGH); // connect to DC register, not eeprom
  digitalWrite(VPRG, LOW); // write to GS
  
  analogReadResolution(RESOLUTION);
  analogReference(EXTERNAL);
  // the greyscale clock needs to be very fast
  // if it is too slow (like, 1MHz) then the audio signal will be full of crap
  analogWriteFrequency(GSCLK, 4000000); // set grayscale clock to 1MHz
  analogWriteResolution(2); // dont need much, 0-15
  randomSeed(analogRead(1)); // not connected
  
  buttonval = 800;
  buttonlast = buttonval;
  buttonder = 0.0;
  
  for (i=0; i<48; i++)
  {
    x = i/36.0;
    colormap[i*3+0] = pow(cos((2./3.)*3.14159*x),2.);
    colormap[i*3+1] = pow(cos(3.14159*(x-0.5)),2.);
    colormap[i*3+2] = pow(cos((2./3.)*3.14159*(1.-x)),2.);
  }
  
  ptr[0] = r;
  ptr[1] = g+4;
  ptr[2] = b+4;
  ptr[3] = r+4;
  ptr[4] = r+5;
  ptr[5] = g+5;
  ptr[6] = b+5;
  ptr[7] = r+6;
  ptr[8] = g+6;
  ptr[9] = b+6;
  ptr[10] = r+7;
  ptr[11] = b+7;
  ptr[12] = g+7;
  ptr[13] = g+8;
  ptr[14] = b+8;
  ptr[15] = r+8;
  ptr[16] = g+3;
  ptr[17] = b+3;
  ptr[18] = r+3;
  ptr[19] = g+0;
  ptr[20] = b+0;
  ptr[21] = r+0;
  ptr[22] = g+0;
  ptr[23] = b+0;
  ptr[24] = r+0;
  ptr[25] = g+0;
  ptr[26] = b+0;
  ptr[27] = r+0;
  ptr[28] = g+0;
  ptr[29] = b+9;
  ptr[30] = g+9;
  ptr[31] = r+9;
  ptr[32] = b+2;
  ptr[33] = g+2;
  ptr[34] = r+2;
  ptr[35] = g+1;
  ptr[36] = b+1;
  ptr[37] = r+1;
  ptr[38] = b+0;
  ptr[39] = g+0;
  ptr[40] = r+0;
  ptr[41] = g+11;
  ptr[42] = b+11;
  ptr[43] = r+11;
  ptr[44] = b+0;
  ptr[45] = b+10;
  ptr[46] = g+10;
  ptr[47] = r+10;

  
  setDC(0xff);
  
  val = 0;
  val2 = 0;
  for (i=0; i<12; i++)
  {
    r[i] = g[i] = b[i] = 0;
    r_b[i] = g_b[i] = b_b[i] = 0;
  }
  
  pattern_setup();
  timerstart();
  startup();
  
  currmode = 0;
  //tuner_setup();
  buttontimer = millis();
  buttontimer2 = millis();
}

void timerstart()
{
  analogWrite(GSCLK, 2);
  Timer3.begin(timerCallback3, 4096);
}

void timerend()
{
  analogWrite(GSCLK, 0);
  Timer3.end();
}

void timerCallback2()
{
  digitalWriteFast(GSCLK, HIGH);
  digitalWriteFast(GSCLK, LOW);
}

void timerCallback3()
{
  digitalWriteFast(BLANK, HIGH);
  digitalWriteFast(BLANK, LOW);
}

void loop()
{ 

  // do things based on what the current mode is
  if (currmode == 0)
  {
    // sleep mode
    patterncallback();
    setGS2(0);
    delay(10);
  } else if (currmode == 1) {
    // tuner mode
    /*
    for (i=0; i<3; i++)
    {
      ftemp = q_mag[i]*k[i];
      r_b[i % 12] = (int)constrain(r_b[i % 12] + (ftemp*colormap[i*3]) - 4,0.,255.);
      g_b[i % 12] = (int)constrain(g_b[i % 12] + (ftemp*colormap[i*3+1]) - 4.5,0.,255.);
      b_b[i % 12] = (int)constrain(b_b[i % 12] + (ftemp*colormap[i*3+2]) - 5,0.,255.);
      
      r[i] = r_b[i];
      g[i] = g_b[i];
      b[i] = b_b[i];
    }
    */
    
    for (i=0; i<12; i++)
    {
      r_b[i] = constrain(0.94*r_b[i] + q_mag[i], 10, 255);
      r[i] = r_b[i];
      g_b[i] = constrain(0.95*g[i] + q_mag[i+12],10,255);
      g[i] = g_b[i];
      b_b[i] = constrain(0.96*b[i] + q_mag[i+24],10,255);
      b[i] = b_b[i];
    }
    
    setGS2(0);
    delay(20);
  } else if (currmode == 2) {
    // party mode
    party_colors();
    setGS2(0);
    delay(10);
  }
  
  // button checking
  if (millis() - buttontimer > 250)
  {
    buttontimer = millis();
    if (button_pressed()) buttonpress();
  }
  
}

// change the mode and flash the lights
void buttonpress()
{
  // increment and wrap
  currmode ++;
  if (currmode == 3) currmode = 0;
  
  // turn on tuner if necessary
  if (currmode == 1) tuner_setup();
  else if (currmode == 2) tuner_kill();
  
  if (currmode == 2) party_setup();
  else if (currmode == 0) party_kill();
  
  for (j=0; j<=255; j+=5)
  {
    for (i=0; i<12; i++)
    {
      r[i] = j;
      g[i] = j;
      b[i] = j;
    }
    setGS2(0);
    delay(5);
  }
  for (j=255; j>=0; j-=3)
  {
    for (i=0; i<12; i++)
    {
      r[i] = j;
      g[i] = j;
      b[i] = j;
    }
    setGS2(0);
    delay(5);
  }
  delay(100);
}

uint8_t button_pressed ()
{
  int i;
  float val = 0.0;
  //for (i=0; i<10; i++)
  //  val += analogRead(BUTTON)/10.;
  //buttonval = 0.9*buttonval + 0.1*analogRead(BUTTON);
  //buttonder = 0.9*buttonder + 0.1*(buttonval-buttonlast);
  //buttonlast = buttonval;

  //Serial.println(buttonval);
  if (analogRead(BUTTON) < 2 && millis() - buttontimer2 > 1000)
  {
    buttontimer2 = millis();
    return 1;
  }
  return 0;
}

void startup()
{
  // clear everything
  for (i=0; i<12; i++)
  {
    r[i] = g[i] = b[i] = 0;
  }
  setGS2(0);
  for (i=0; i<12; i++)
  {
    r[i] = 255;
    setGS2(0);
    delay(30);
  }
  delay(30);
  for (i=0; i<12; i++)
  {
    r[i] = g[i] = b[i] = 0;
  }
  setGS2(0);
  for (i=0; i<12; i++)
  {
    g[i] = 255;
    setGS2(0);
    delay(30);
  }
  delay(30);
  for (i=0; i<12; i++)
  {
    r[i] = g[i] = b[i] = 0;
  }
  setGS2(0);
  for (i=0; i<12; i++)
  {
    b[i] = 255;
    setGS2(0);
    delay(30);
  }
  delay(30);
  for (i=0; i<12; i++)
  {
    r[i] = g[i] = b[i] = 0;
  }
  setGS2(0);
  for (i=0; i<12; i++)
  {
    r[i] = g[i] = b[i] = 255;
    setGS2(0);
    delay(30);
  }
  for (j=255; j>=0; j--)
  {
    for (i=0; i<12; i++)
    {
      r[i] = j;
      g[i] = j;
      b[i] = j;
      setGS2(0);
    }
    delay(5);
  }
}

// for only using 8-bit ints
void setGS2(uint8_t v)
{
  uint8_t val;
  //timerend();
  digitalWriteFast(VPRG,0);
  for ( uint8_t i = 0; i < 48; i++)
  {
    val = *(ptr[i]);
//    val = exp_map[val];
//    shift12(((uint16_t)val)<<4);
    shift12(exp_map2[val]);
  }
  digitalWriteFast(XLAT,1);
  digitalWriteFast(XLAT,0);
  if ( needPulse) {
    digitalWriteFast(SCLK,1);
    digitalWriteFast(SCLK,0);
    
    // For some reason, it doesn't work right if I clear needPulse, my reading
    // of the datasheet says I should clear it, but then the bits get off.
    //needPulse = 0;
  }
  //timerstart();
}

void setDC(uint8_t v)
{
  digitalWriteFast(VPRG,1);
  for ( uint8_t i = 0; i < 16*3; i++) shift6(v);
  digitalWriteFast(XLAT,1);
  digitalWriteFast(XLAT,0);
  digitalWriteFast(VPRG,0);

  needPulse = 1;
}

uint8_t shift6(uint8_t v)
{
  uint8_t r=0;

  for (uint8_t m = 0x20; m; m >>= 1) {
    digitalWriteFast(SIN, ((v&m) ? 1 : 0));
    digitalWriteFast(SCLK, 1);
    digitalWriteFast(SCLK, 0);

  }
  return r;
}

uint8_t shift8(uint8_t v)
{
  uint8_t r=0;

  for (uint8_t m = 0x80; m; m >>= 1) {
    digitalWriteFast(SIN, ((v&m) ? 1 : 0));
    digitalWriteFast(SCLK, 1);
    digitalWriteFast(SCLK, 0);

  }
  return r;
}

uint16_t shift12(uint16_t v)
{
  uint16_t r=0;

  for (uint16_t m = 0x0800; m; m >>= 1) {
    digitalWriteFast(SIN, ((v&m) ? 1 : 0));
    digitalWriteFast(SCLK, 1);
    digitalWriteFast(SCLK, 0);
  }
  return r;
}
