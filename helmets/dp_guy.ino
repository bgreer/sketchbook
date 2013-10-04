#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NPIX 6 /* number of lights in each mini-strip */
#define PATTERN_MIN 40

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8*NPIX*2, PIN, NEO_GRB + NEO_KHZ800);

// colors for each side
uint8_t lr[8], lg[8], lb[8];
uint8_t rr[8], rg[8], rb[8];
uint8_t expcor[256]; // exponential correction lookup table

// for new mode
float pos[3];
float dir[3];
float colval[3];
int mult[3];

float dim = 2.0;

void setup()
{
	uint8_t ii;
	randomSeed(analogRead(0));
	Serial.begin(115200);
	Serial.println("building table..");
	build_table(255.0);
	Serial.println("initializing strip..");
  strip.begin();
  blank();
	mapcolors();
  strip.show();
	delay(500);

	// init the strand now with the default colors
	for (ii=0; ii<8; ii++)
	{
		defaultcolors(ii, lr+ii, lg+ii, lb+ii);
		defaultcolors(ii, rr+ii, rg+ii, rb+ii);
	}
	Serial.println(lg[0]);
	mapcolors();
	strip.show();

	test(10);

  pos[0] = 0.0;
  pos[1] = 3.0;
  pos[2] = 5.0;
  
  colval[0] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[0] = 0.21;
  mult[0] = random(4);
  
  colval[1] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[1] = 0.2;
  mult[1] = random(4);
  
  colval[2] = random(256-PATTERN_MIN)+PATTERN_MIN;
  dir[2] = 0.3;
  mult[2] = random(4);
}

void loop()
{
	uint8_t ii, ij;

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

		lr[ij] = (uint8_t)((int)(colval[0]*pow(sin(mult[0]*PI*(ij+8-pos[0])/16.),2.)));
    lg[ij] = (uint8_t)((int)(colval[1]*pow(sin(mult[1]*PI*(ij+8-pos[1])/16.),2.)));
    lb[ij] = (uint8_t)((int)(colval[2]*pow(sin(mult[2]*PI*(ij+8-pos[2])/16.),2.)));
  }
	mapcolors();
	strip.show();

	delay(10);
/*
	for (ii=2; ii<=4; ii++)
	{
		swipeup(ii, 70);
		swipedown(ii,70);
	}
	delay(100);
	*/
/*
	dim = 26.0;
	for (ii=0; ii<8; ii++)
	{
		defaultcolors(ii, lr+ii, lg+ii, lb+ii);
		defaultcolors(ii, rr+ii, rg+ii, rb+ii);
	}
	mapcolors();
	strip.show();
	delay(50);
	for (ii=0; ii<50; ii++)
	{
		dim *= 0.95;
		mapcolors();
		strip.show();
		delay(10);
	}
	delay(100);
	for (ii=0; ii<50; ii++)
	{
		dim /= 0.95;
		mapcolors();
		strip.show();
		delay(10);
	}
	dim = 2.0;
	blank();
	mapcolors();
	strip.show();
	delay(300);
*/
/*
	stack(60);
	delay(200);
	unstack(60);
	delay(200);

	for (ii=0; ii<5; ii++)
	{
		swipeup(random(2)+1,70);
		delay(50);
	}

	delay(100);

	for (ii=0; ii<5; ii++)
		flash(200);

	delay(100);
	*/
}

void test(int d)
{
	uint8_t ii;
	blank();
	mapcolors();
	strip.show();
	delay(d);
	for (ii=0; ii<8; ii++)
	{
		rr[ii] = lr[ii] = 255;
		mapcolors();
		strip.show();
		delay(d);
	}
	blank();
	mapcolors();
	strip.show();
	delay(d);
	for (ii=0; ii<8; ii++)
	{
		rg[ii] = lg[ii] = 255;
		mapcolors();
		strip.show();
		delay(d);
	}
	blank();
	mapcolors();
	strip.show();
	delay(d);
	for (ii=0; ii<8; ii++)
	{
		rb[ii] = lb[ii] = 255;
		mapcolors();
		strip.show();
		delay(d);
	}
	blank();
	mapcolors();
	strip.show();
	delay(d);

}

void flash(int d)
{
	uint8_t ii,ij;
	for (ii=0; ii<8; ii++)
	{
		if (ii%2==0)
		{
			defaultcolors(ii, rr+ii, rg+ii, rb+ii);
			defaultcolors(ii, lr+ii, lg+ii, lb+ii);
		} else {
			rr[ii] = rg[ii] = rb[ii] = 0;
			lr[ii] = lg[ii] = lb[ii] = 0;
		}
	}
	mapcolors();
	strip.show();
	delay(d);
	for (ii=0; ii<8; ii++)
	{
		if (ii%2==1)
		{
			defaultcolors(ii, rr+ii, rg+ii, rb+ii);
			defaultcolors(ii, lr+ii, lg+ii, lb+ii);
		} else {
			rr[ii] = rg[ii] = rb[ii] = 0;
			lr[ii] = lg[ii] = lb[ii] = 0;
		}
	}
	mapcolors();
	strip.show();
	delay(d);
}

// unstack colors
void unstack(int d)
{
	uint8_t stacked = 8;
	int8_t pos;

	while (stacked > 0)
	{
		pos = 8-stacked;
		stacked --;
		while (pos >= 0)
		{
			pos --;
			stackhelper2(stacked, pos);
			delay(d);
		}
		stackhelper2(stacked, pos);
		delay(d);
	}
}

void stackhelper2 (uint8_t stacked, int8_t pos)
{
	uint8_t ii;
	for (ii=0; ii<8; ii++)
	{
		if (ii >= 8-stacked)
		{
			defaultcolors(ii, rr+ii, rg+ii, rb+ii);
			defaultcolors(ii, lr+ii, lg+ii, lb+ii);
		} else if (ii == pos) {
			defaultcolors(7-stacked, rr+ii, rg+ii, rb+ii);
			defaultcolors(7-stacked, lr+ii, lg+ii, lb+ii);
    } else {
			rr[ii] = rg[ii] = rb[ii] = 0;
			lr[ii] = lg[ii] = lb[ii] = 0;
		}
	}
	mapcolors();
	strip.show();
}


// stack lights
void stack(int d)
{
	uint8_t stacked = 0;
	uint8_t pos = 8;

	while (stacked < 8)
	{
		// stack a new light
		pos = 8;
		while (pos > stacked)
		{
			pos --;
			stackhelper(stacked, pos);
			delay(d);
		}
		pos = 8;
		stacked ++;
		stackhelper(stacked, pos);
		delay(d);
	}
}

void stackhelper (uint8_t stacked, uint8_t pos)
{
	uint8_t ii;
	for (ii=0; ii<8; ii++)
	{
		if (ii < stacked)
		{
			defaultcolors(ii, rr+ii, rg+ii, rb+ii);
			defaultcolors(ii, lr+ii, lg+ii, lb+ii);
		} else if (ii == pos) {
			defaultcolors(stacked, rr+ii, rg+ii, rb+ii);
			defaultcolors(stacked, lr+ii, lg+ii, lb+ii);
    } else {
			rr[ii] = rg[ii] = rb[ii] = 0;
			lr[ii] = lg[ii] = lb[ii] = 0;
		}
	}
	mapcolors();
	strip.show();
}

void swipedown(uint8_t width, int d)
{
  int8_t offset = 0;
  int8_t ii;
  
  for (offset = 6+width; offset>=0; offset--)
  {
    // set each light
    for (ii=0; ii<8; ii++)
    {
      // if within the swipe
      if (ii <= offset && ii > offset-width)
      {
        defaultcolors(ii, rr+ii, rg+ii, rb+ii);
				defaultcolors(ii, lr+ii, lg+ii, lb+ii);
      } else {
				rr[ii] = rg[ii] = rb[ii] = 0;
				lr[ii] = lg[ii] = lb[ii] = 0;
			}
			mapcolors();
			strip.show();
    }
		delay(d);
  }
}

// swipe a band up or down, width min = 1, max = 8
void swipeup(uint8_t width, int d)
{
  int8_t offset = 0;
  int8_t ii;
  
  for (offset = 0; offset<7+width; offset++)
  {
    // set each light
    for (ii=0; ii<8; ii++)
    {
      // if within the swipe
      if (ii <= offset && ii > offset-width)
      {
        defaultcolors(ii, rr+ii, rg+ii, rb+ii);
				defaultcolors(ii, lr+ii, lg+ii, lb+ii);
      } else {
				rr[ii] = rg[ii] = rb[ii] = 0;
				lr[ii] = lg[ii] = lb[ii] = 0;
			}
			mapcolors();
			strip.show();
    }
		delay(d);
  }
}

// take the color arrays and map them to the neopixel buffer
void mapcolors()
{
  uint8_t ii, ij;
  
  for (ii=0; ii<8; ii++)
  {
    for (ij=0; ij<NPIX; ij++)
    {
      strip.setPixelColor(ii*NPIX+ij, expcor[(uint8_t)(rr[ii]/dim)], expcor[(uint8_t)(rg[ii]/dim)], expcor[(uint8_t)(rb[ii]/dim)]);
      strip.setPixelColor((ii+8)*NPIX+ij, expcor[(uint8_t)(lr[ii]/dim)], expcor[(uint8_t)(lg[ii]/dim)], expcor[(uint8_t)(lb[ii]/dim)]);
    }
  }
  
  strip.show();
}


// for each of the 8 strips, figure out the default rainbow colors
// puts the correct rgb values into r,g,b, wherever they may point
void defaultcolors(uint8_t index, uint8_t *r, uint8_t *g, uint8_t *b)
{
  *r = 0;
  *g = 0;
  *b = 0;
  switch (index)
  {
    case 0:
      *r = 255;
      break;
    case 1:
      *r = 240;
      *g = 120;
      break;
    case 2:
      *r = 200;
      *g = 200;
      break;
    case 3:
      *r = 70;
      *g = 220;
      *b = 30;
      break;
    case 4:
      *g = 220;
      *b = 100;
      break;
    case 5:
      *g = 220;
      *b = 220;
      break;
    case 6:
			*g = 30;
      *b = 220;
      *r = 120;
      break;
    case 7:
      *b = 200;
      *r = 200;
      break;
  }
}

// sets all lights to black
void blank()
{
  uint8_t ii;
  for (ii=0; ii<8; ii++)
  {
    lr[ii] = rr[ii] = 0;
    lg[ii] = rg[ii] = 0;
    lb[ii] = rb[ii] = 0;
  }
}

// put values intio the corrective lookup table
/* math:

	corr = b[exp(ax)-1]

	255 = b[exp(a*255)-1]
	b = 255/[exp(a*255)-1]

*/
void build_table(float gain)
{
	uint8_t ii;
	float b = 255./(exp(255/gain)-1.0);

	for (ii=0; ii<=254; ii++)
	{
		expcor[ii] = (uint8_t)(b*(exp(ii/gain)-1.0));
	}
	expcor[0] = 0;
	expcor[255] = 255;
}
