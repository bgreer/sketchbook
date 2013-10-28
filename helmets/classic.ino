// this is for the guy helmet

void classic_init()
{

}

void classic_step()
{
	uint8_t ii;
	for (ii=2; ii<=4; ii++)
	{
		swipeup(ii, 70);
		swipedown(ii,70);
	}
	delay(100);
	

	dim = 26.0;
	for (ii=0; ii<8; ii++)
	{
		defaultcolors(ii, lr+ii, lg+ii, lb+ii);
		defaultcolors(ii, rr+ii, rg+ii, rb+ii);
	}
	mapcolors();
	delay(50);
	for (ii=0; ii<50; ii++)
	{
		dim *= 0.95;
		mapcolors();
		delay(10);
	}
	delay(100);
	for (ii=0; ii<50; ii++)
	{
		dim /= 0.95;
		mapcolors();
		delay(10);
	}
	dim = 1.0;
	blank();
	mapcolors();
	delay(300);


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
    }
		delay(d);
  }
}

