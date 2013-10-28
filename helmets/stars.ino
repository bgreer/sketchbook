// this is for the thomas helmet
#define NUMSTARS 40

float spos[NUMSTARS][3];

void stars_init()
{
	uint8_t ii;
	for (ii=0; ii<NUMSTARS; ii++)
	{
		randompos(ii);
		spos[ii][2] = random(1000)*5.0/999.0;
	}

	shiftpos = 48;
}

void stars_step()
{
	uint8_t ii, col, row;

	// move every star forward a bit
	for (ii=0; ii<NUMSTARS; ii++)
	{
		spos[ii][2] -= 0.01;
		// check for stars moving past view plane
		if (spos[ii][2] < 0.1)
		{
			randompos(ii);
		}
	}

	// clear pixel buffer
	for (ii=0; ii<40; ii++)
		pix[ii] = 0x00;

	// render frame
	for (ii=0; ii<NUMSTARS; ii++)
	{
		col = (uint8_t)(spos[ii][0]*2.0/spos[ii][2]) + 20;
		row = (uint8_t)(spos[ii][1]*2.0/spos[ii][2]) + 4;

		if (col >= 0 && col < 40 && row >= 0 && row < 8)
		{
			pix[col] |= (0x01) << row;
		}
	}

	// call refresh
	refresh = 1;
}

void randompos(uint8_t index)
{
	spos[index][0] = ((random(1000)/999.0)-0.5)*80.0;
	spos[index][1] = ((random(1000)/999.0)-0.5)*16.0;
	spos[index][2] = 5.0;
}
