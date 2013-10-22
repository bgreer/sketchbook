
#define DATA 11
#define LOAD 12
#define CLOCK 13
#define BUFFLEN 48

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
uint8_t w[12];
uint8_t ii, ij, val, shiftpos, refresh, mode, pos;
uint32_t lastupdate, rate, lastbutton;

uint8_t chars[12][7] = {
	{0x7c,0x7e,0x13,0x11,0x13,0x7e,0x7c}, // A
	{0x7f,0x7f,0x49,0x49,0x49,0x7f,0x36}, // B
	{0x7f,0x7f,0x49,0x49,0x49,0x49,0x41}, // E
	{0x7f,0x7f,0x08,0x08,0x08,0x7f,0x7f}, // H
	{0x7f,0x7f,0x0e,0x1c,0x0e,0x7f,0x7f}, // M
	{0x7f,0x7f,0x0e,0x1c,0x38,0x7f,0x7f}, // N
	{0x3e,0x7f,0x41,0x41,0x41,0x7f,0x3e}, // O
	{0x7f,0x7f,0x11,0x31,0x79,0x6f,0x4e}, // R
	{0x26,0x6f,0x49,0x49,0x4B,0x7A,0x30}, // S
	{0x01,0x01,0x7f,0x7f,0x01,0x01,0x00}, // T
	{0x3f,0x7f,0x40,0x40,0x40,0x7f,0x3f}, // U
	{0x00,0x07,0x0F,0x78,0x78,0x0F,0x07}}; // Y



void setup ()
{
	uint8_t val;
	randomSeed(analogRead(0));
	Serial.begin(115200);
	// initialize
	for (ii=0; ii<BUFFLEN; ii++)
		pix[ii] = 0x00;

	mode = 1;
	pinMode(CLOCK, OUTPUT);
	pinMode(LOAD, OUTPUT);
	pinMode(DATA, OUTPUT);

	initMax();

	for (ii=0x01; ii<0x09; ii++)
		put(ii, 0x00);

	refresh = 1;
	rate = 40;
	shiftpos = 0;
	// ROBOT
	w[0] = 7;
	w[1] = 6;
	w[2] = 1;
	w[3] = 6;
	w[4] = 9;
	// HUMAN
	/*
	w[0] = 3;
	w[1] = 10;
	w[2] = 4;
	w[3] = 0;
	w[4] = 5;
	*/
	drawWord(5);
}

void loop()
{
	// shift placement of buffer on screen by a column
	if (mode == 1)
	{
		if (millis() - lastupdate > rate)
		{
			shiftpos++;
			if (shiftpos > 40+BUFFLEN-1) shiftpos = 0;
			lastupdate = millis();
			refresh = 1;
		}
	} else if (mode == 0) {
		if (millis() - lastupdate > 50)
		{
			if (random(10) < 8)
			{
				int col = random(16);
				int row = random(16);
				pix[col] = pix[col] ^ (0x01)<<random(8);
				pix[col] = pix[col] >> 1;
				pix[row] = pix[row] << 1;
			} else {
				int col = random(15);
				uint8_t swap = pix[col];
				pix[col] = pix[col+1];
				pix[col+1] = swap;
			}

			lastupdate = millis();
			refresh = 1;
			shiftpos = 25;
		}
	}

	if (refresh)
	{
		// update the MAX drivers
		// advanced: update each driver simultaneously
		for (ii=0; ii<8; ii++) // loop over col within each driver
		{
			digitalWrite(LOAD, LOW);
			for (ij=0; ij<2; ij++) // loop over drivers, loading shift line
			{
				// if ij*8+ii+shiftpos is on buffer, grab that value
				// else, grab a 0
				pos = shiftpos - ii + ij*8 - 16;
				if (pos <= BUFFLEN-1 && pos >= 0) val = pix[pos];
				else val = 0;
				// shift out register ii, value
				shift(ii+1);
				shift_rev(val);
			}
			// at this point, i have shifted out 6 values
			// so that each driver will be updated upon latch
			digitalWrite(LOAD, LOW);
			digitalWrite(LOAD, HIGH);
		}
		refresh = 0;
	}

	// check for button press
	if (0 && millis()-lastbutton > 250)
	{
		lastbutton = millis();
		mode++;
		if (mode>1) mode = 0;
	}
}

void put(uint8_t r, uint8_t v)
{
	digitalWrite(LOAD, LOW);
	shift(r);
	shift(v);
	digitalWrite(LOAD, LOW);
	digitalWrite(LOAD, HIGH);
}

// todo: replace with port manipulation
void shift(uint8_t val)
{
	uint8_t ik;
	for (ik=8; ik>=1; ik--)
	{
		digitalWrite(CLOCK, LOW);
		if (val & (0x01<<(ik-1))) digitalWrite(DATA, HIGH);
		else digitalWrite(DATA, LOW);
		digitalWrite(CLOCK, HIGH);
	}
}

void shift_rev(uint8_t val)
{
	uint8_t ik;
	for (ik=8; ik>=1; ik--)
	{
		digitalWrite(CLOCK, LOW);
		if (val & (0x01<<(8-ik))) digitalWrite(DATA, HIGH);
		else digitalWrite(DATA, LOW);
		digitalWrite(CLOCK, HIGH);
	}
}


void drawWord(uint8_t len)
{
	uint8_t curr = 0, ij;
	for (ii=0; ii<len; ii++)
	{
		// assume characters are 7 cols wide
		for (ij=0; ij<7; ij++)
		{
			if (curr < BUFFLEN) pix[curr] = chars[w[ii]][ij];
			Serial.println(pix[curr]);
			curr++;
		}
		curr+=2; // add a blank col
	}
	// clear the rest of the pixel buffer
	while (curr < BUFFLEN-1) pix[curr++] = 0x00;
}

// not sure if this is actually needed:
// shift everything in the pixel buffer horizontally by one pixel
void shiftDisplay(uint8_t val)
{
	for (ii=39; ii>0; ii--)
		pix[ii] = pix[ii-1];
	pix[0] = val;
}

void initMax()
{
	put(0x09, 0x00); // decide mode
	put(0x0B, 0x07); // scan limit
	put(0x0C, 0x01); // shutdown
	put(0x0F, 0x00); // display test
	put(0x0A, 0x0f); // intensity
}

