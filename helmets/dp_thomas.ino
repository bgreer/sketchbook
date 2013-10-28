#include "font.h"
#define BUTTON 2
#define LED 13
#define DATA 11
#define LOAD 12
#define CLOCK 10
#define BUFFLEN 200

#define DHIGH {PORTB |= B00001000;}
#define DLOW {PORTB &= B11110111;}
#define CHIGH {PORTB |= B00000100;}
#define CLOW {PORTB &= B11111011;}
#define LHIGH {PORTB |= B00010000;}
#define LLOW {PORTB &= B11101111;}

// words: ROBOT HUMAN YES NO
// unique characters: ROBTHUMANYES
// sorted: ABEHMNORSTUY (index starts at 0)
//
// ROBOT = 7 6 1 6 9
// HUMAN = 3 10 4 0 5
// YES = 11 2 8
// NO = 5 6

// pixel buffer
uint8_t pix[BUFFLEN];
char *w;
volatile uint8_t mode;
uint8_t ii, ij, val, refresh, lastmode, scrollmode;
uint32_t lastupdate, rate, lastbutton;
int16_t shiftpos, pos;

void setup ()
{
	uint8_t val;
	randomSeed(analogRead(0));
	Serial.begin(115200);
	// initialize
	for (ii=0; ii<BUFFLEN; ii++)
		pix[ii] = 0x00;

	lastmode = -1;
	mode = 0;
	pinMode(CLOCK, OUTPUT);
	pinMode(LOAD, OUTPUT);
	pinMode(DATA, OUTPUT);
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	digitalWrite(CLOCK, LOW);
	digitalWrite(LOAD, LOW);
//	digitalWrite(DATA, LOW);
	DLOW

	// set up button
	pinMode(BUTTON, INPUT);
	digitalWrite(BUTTON, HIGH); // internal pullup
	attachInterrupt(0, buttonCallback, FALLING);

	initMax();

	refresh = 1;
	rate = 30;
	shiftpos = 0;
	// ROBOT
	w = m2;

	drawWord();

	for (ii=0; ii<5; ii++)
	{
		shift(0x07);
		shift_rev(0x7E);
	}
}

void buttonCallback()
{
	if (millis() > 500 && millis()-lastbutton > 250)
	{
		mode ++;
		if (mode > 5) mode = 0;
		digitalWrite(LED, mode);
		lastbutton = millis();
	}
}

void loop()
{
	// shift placement of buffer on screen by a column
	if (mode == 0)
	{
		if (lastmode != mode)
		{
			// set up
			for (ii=0; ii<BUFFLEN; ii++) pix[ii] = 0x00;
			shiftpos = 47;
			lastmode = mode;
			refresh = 1;
		}
	} else if (mode == 1 || mode == 3) {
		if (lastmode != mode)
		{
			// set up
			shiftpos = 0;
			if (mode == 3) w = m2; // around the world
			if (mode == 1) w = m0; // robot
			scrollmode = 0;
			drawWord();
			lastmode = mode;
		}
		if (millis() - lastupdate > rate)
		{
			shiftpos++;
			if (shiftpos > 40+BUFFLEN-1)
			{
				shiftpos = 0;
				// swap word
				if (mode == 1 && random(2))
				{
					if (scrollmode) w = m0;
					else w = m1;
					scrollmode ^= 0x01;
				}
			}
			lastupdate = millis();
			drawWord();
			refresh = 1;
		}
	} else if (mode == 2) {
		if (lastmode != mode)
		{
			// set up
			w = m0;
			drawWord();
			lastmode = mode;
		}
		if (millis() - lastupdate > 10)
		{
			if (random(10) < 8)
			{
				int col = random(40);
				int row = random(40);
				pix[col] = pix[col] ^ (0x01)<<random(8);
				pix[col] = pix[col] >> 1;
				pix[row] = pix[row] << 1;
			} else {
				int col = random(39);
				uint8_t swap = pix[col];
				pix[col] = pix[col+1];
				pix[col+1] = swap;
			}

			lastupdate = millis();
			refresh = 1;
			shiftpos = 48;
		}
	} else if (mode == 4) {
		// stars
		if (lastmode != mode)
		{
			stars_init();
			lastmode = mode;
		}
		if (millis() - lastupdate > 10)
		{
			stars_step();
			lastupdate = millis();
		}
	} else if (mode == 5) {
		// pong
		if (lastmode != mode)
		{
			pong_init();
			lastmode = mode;
		}
		if (millis() - lastupdate > 10)
		{
			pong_step();
			lastupdate = millis();
		}

	}

	if (refresh)
	{

		initMax(); // THIS FIXES IT
		// update the MAX drivers
		// advanced: update each driver simultaneously
		for (ii=0; ii<8; ii++) // loop over col within each driver
		{
			//digitalWrite(LOAD, LOW);
			for (ij=0; ij<5; ij++) // loop over drivers, loading shift line
			{
				// if ij*8+ii+shiftpos is on buffer, grab that value
				// else, grab a 0
				pos = shiftpos - ii + ij*8 - 40;
				if (pos < BUFFLEN && pos >= 0) val = pix[pos] & 0xFF;
				else val = 0;
				// shift out register ii, value
				shift(ii+1);
				shift_rev(val);
			}
			// at this point, i have shifted out 6 values
			// so that each driver will be updated upon latch
			latch();
		}

		// basic: update each driver independently
		for (ii=0; ii<8; ii++)
		{
			
		}

		refresh = 0;
	}

}


// todo: replace with port manipulation
void shift(uint8_t val)
{
	uint8_t ik;
	if (val > 0x08) Serial.println(val);
	for (ik=8; ik>=1; ik--)
	{
		if (val & (0x01<<(ik-1))) DHIGH //digitalWrite(DATA, HIGH);
		else DLOW //digitalWrite(DATA, LOW);
		CHIGH //digitalWrite(CLOCK, HIGH);
		CLOW //digitalWrite(CLOCK, LOW);
	}
}

void shift_rev(uint8_t val)
{
	uint8_t ik;
	for (ik=8; ik>=1; ik--)
	{
		if (val & (0x01<<(8-ik))) DHIGH //digitalWrite(DATA, HIGH);
		else DLOW //digitalWrite(DATA, LOW);
		CHIGH //digitalWrite(CLOCK, HIGH);
		CLOW //digitalWrite(CLOCK, LOW);
	}
}


void drawWord()
{
	uint8_t curr = 0, ij;
	ii = 0;
	for (ii=0; w[ii] != '\0'; ii++)
	{
		// assume characters are 7 cols wide
		for (ij=0; ij<8; ij++)
		{
			if (curr < BUFFLEN) pix[curr] = reverse_byte(font_c64[(uint8_t)(w[ii])-32][ij]);
			curr++;
		}
//		curr+=2; // add a blank col
	}
	// clear the rest of the pixel buffer
	while (curr < BUFFLEN-1) pix[curr++] = 0x00;
}

uint8_t reverse_byte (uint8_t in)
{
	uint8_t ret, ii;
	ret = 0x00;
	for (ii=0; ii<8; ii++)
		ret |= (0x01 & (0x01 & (in>>ii))) << (7-ii);
	return ret;
}

// not sure if this is actually needed:
// shift everything in the pixel buffer horizontally by one pixel
void shiftDisplay(uint8_t val)
{
	for (ii=39; ii>0; ii--)
		pix[ii] = pix[ii-1];
	pix[0] = val;
}

void latch()
{
	LHIGH //digitalWrite(LOAD, HIGH);
	LLOW //digitalWrite(LOAD, LOW);
}

void put_all(uint8_t val, uint8_t val2)
{
	uint8_t ik;
	for (ik=0; ik<5; ik++)
	{
		shift(val);
		shift(val2);
	}
	latch();
}

void initMax()
{
	put_all(0x09, 0x00); // decode mode
	put_all(0x0B, 0x07); // scan limit
	put_all(0x0C, 0x01); // shutdown
	put_all(0x0F, 0x00); // display test
	put_all(0x0A, 0x0F); // intensity
}

