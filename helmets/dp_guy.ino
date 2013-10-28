#include <Adafruit_NeoPixel.h>

#define NUMMODES 3
#define MODE_OFF 0
#define MODE_CLASSIC 1
#define MODE_PATTERN 2

#define LED 13
#define BUTTON 2
#define DATALEFT 6
#define DATARIGHT 7
#define NPIX 6 /* number of lights in each mini-strip */
#define PATTERN_MIN 40

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8*NPIX, DATALEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(8*NPIX, DATARIGHT, NEO_GRB + NEO_KHZ800);

volatile uint8_t mode;
uint8_t lastmode;
uint32_t lastupdate;

// colors for each side
uint8_t lr[8], lg[8], lb[8];
uint8_t rr[8], rg[8], rb[8];
uint8_t expcor[256]; // exponential correction lookup table

float dim = 1.0;

void setup()
{
	uint8_t ii;
	randomSeed(analogRead(0));
	Serial.begin(115200);
	// build exponential correction table
	build_table(255.0);
	// begin neopixel driver
  strip.begin();
	strip2.begin();

  blank();
	mapcolors();
	mode = 0;
	lastmode = -1;

	// set up button
	pinMode(BUTTON, INPUT);
	digitalWrite(BUTTON, HIGH); // internal pullup
	attachInterrupt(0, buttonCallback, FALLING);

	dim = 2.0;

}

void buttonCallback()
{
	if (millis() > 500 && millis()-lastbutton > 250)
	{
		mode ++;
		if (mode == NUMMODES) mode = 0;
		digitalWrite(LED, mode);
		lastbutton = millis();
	}
}


void loop()
{
	uint8_t ii, ij;

	if (mode==MODE_OFF)
	{
		if (lastmode != mode)
		{
			blank();
			mapcolors();
			lastmode = mode;
		}
	} else if (mode==MODE_CLASSIC) {
		if (lastmode != mode)
		{
			pattern_init();
			lastmode = mode;
		}
		if (millis() - lastupdate > 10)
		{
			pattern_step();
			lastupdate = millis();
		}
	} else if (mode==MODE_PATTERN){
		if (lastmode != mode)
		{
			classic_init()
			lastmode = mode;
		}
		classic_step();
	}
	delay(1);
}

void test(int d)
{
	uint8_t ii;
	blank();
	mapcolors();
	delay(d);
	for (ii=0; ii<8; ii++)
	{
		rr[ii] = lr[ii] = 255;
		mapcolors();
		delay(d);
	}
	blank();
	mapcolors();
	delay(d);
	for (ii=0; ii<8; ii++)
	{
		rg[ii] = lg[ii] = 255;
		mapcolors();
		delay(d);
	}
	blank();
	mapcolors();
	delay(d);
	for (ii=0; ii<8; ii++)
	{
		rb[ii] = lb[ii] = 255;
		mapcolors();
		delay(d);
	}
	blank();
	mapcolors();
	delay(d);

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
      strip2.setPixelColor(ii*NPIX+ij, expcor[(uint8_t)(lr[ii]/dim)], expcor[(uint8_t)(lg[ii]/dim)], expcor[(uint8_t)(lb[ii]/dim)]);
    }
  }
  
  strip.show();
	strip2.show();
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
