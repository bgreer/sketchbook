
/* to clear atmega:
/home/bgreer/arduino-1.0.3/hardware/tools/avrdude -C/home/bgreer/arduino-1.0.3/hardware/tools/avrdude.conf -v -v -v -v -patmega328p -cstk500v1 -P/dev/ttyACM0 -b57600 -e
*/

#define DIAL1 A0
#define DIAL2 A1
#define DIAL3 A2

#define SHUTTER 13

#define PRESS_SHUTTER {PORTB |= B00100000;}
#define RELEASE_SHUTTER {PORTB &= B11011111;}

int dial1, dial2, dial3;
int lastdial1, lastdial2, lastdial3;
int thresh;
long numpics, picsleft;
float exposuretime, timedelay; // in seconds
uint8_t mode;
uint8_t digit[8];
uint8_t ii;
uint8_t j;
uint32_t time, lastchange;
int counter = 0;

void setup()
{
  pinMode(SHUTTER, OUTPUT);
  for (j=0; j<8; j++)
    digit[j] = j+1;
	// set pinmodes to output
  DDRB |= B11011110;
  DDRD |= B01111110;
	// setup timer
  TCCR1B=1; //set no prescaller
  TIMSK1=(1<<TOIE1); //enable timer1 over-flow interrupt
  sei(); //enable global interrupts
  
  ii = 0;
  mode = 0;
  numpics = 0;
  exposuretime = 0.001;
  thresh = 50;
}

void loop()
{
  // read dial values and compute moving average for smoothness
  dial1 = 0.95*dial1 + 0.05*analogRead(DIAL1);
  dial2 = 0.95*dial2 + 0.05*analogRead(DIAL2);
  dial3 = 0.95*dial3 + 0.05*analogRead(DIAL3);
  
  switch (mode)
  {
    case 0:
      convert(picsleft);
      break;
    case 1:
      // exposure time
      if (dial1 > 1022)
        exposuretime = -1;
      else
        exposuretime = 0.001*exp(dial1/70.);
      convert((int)exposuretime*100+mode);
      lastdial1 = dial1;
      break;
    case 2:
      // delay
      if (dial2 > 1022)
        timedelay = -1;
      else
        timedelay = 0.001*exp(dial2/70.);
      convert((int)timedelay*100+mode);
      lastdial2 = dial2;
      break;
    case 3:
      // how many pictures to take
      if (dial3 < 20)
        numpics = 0;
      else if (dial3 > 1022)
        numpics = -1;
      else
        numpics = (int)exp((dial3-20.)/150.);
      convert((int)numpics*100+mode);
      lastdial3 = dial3;
      break;
  }
  
  // if a dial is being moved, change mode
  if (abs(lastdial1-dial1) >= thresh)
  {
    mode = 1;
    thresh = 2;
    lastdial1 = dial1;
    lastchange = millis();
  }
  if (abs(lastdial2-dial2) >= thresh)
  {
    mode = 2;
    thresh = 2;
    lastdial2 = dial2;
    lastchange = millis();
  }
  if (abs(lastdial3-dial3) >= thresh)
  {
    mode = 3;
    thresh = 2;
    lastdial3 = dial3;
    lastchange = millis();
  }
  
  if (millis() - lastchange > 5000)
  {
    mode = 0; // picture taking mode
    thresh = 50;
    picsleft = numpics;
  }
  
}

// go from an integer (1234) to a digit array (10 10 10 10 1 2 3 4)
void convert(int val)
{
  for (j=0; j<8; j++)
  {
    if (val == 0)
      digit[7-j] = 10;
    else
      digit[7-j] = val%10;
    val /= 10;
  }
}

// timer loop
// used to switch between characters quickly
//  so that they all appear on
ISR (TIMER1_OVF_vect)
{
  // turn off display
  PORTB &= B11111101;
  // move character
  ii ++;
  if (ii == 8) ii=0;
  PORTB &= B11100011; // clear character selection
	// set IO lines to select current character
  PORTB |= (ii&B00000001)<<4; // PB4
  PORTB |= (ii&B00000010)<<2; // PB3
  PORTB |= (ii&B00000100); // PB2
  // clear digit
  PORTD &= B10000001;
  PORTB &= B00111111;
  // set character segments
  setDigit(digit[ii]);
  // turn display back on
  PORTB |= B00000010;
}

/*
PD5 = decimal point
PD6 = bottom right
PB7 = top
PB6 = bottom left
PD4 = bottom
PD3 = middle
PD2 = top right
PD1 = top left
*/

void setDigit(uint8_t num)
{
  switch (num)
  {
    case 0:
      PORTD |= B01010110;
      PORTB |= B11000000;
      break;
    case 1:
      PORTD |= B01000100;
      PORTB |= B00000000;
      break;
    case 2:
      PORTD |= B00011100;
      PORTB |= B11000000;
      break;
    case 3:
      PORTD |= B01011100;
      PORTB |= B10000000;
      break;
    case 4:
      PORTD |= B01001110;
      PORTB |= B00000000;
      break;
    case 5:
      PORTD |= B01011010;
      PORTB |= B10000000;
      break;
    case 6:
      PORTD |= B01011010;
      PORTB |= B11000000;
      break;
    case 7:
      PORTD |= B01000100;
      PORTB |= B10000000;
      break;
    case 8:
      PORTD |= B01011110;
      PORTB |= B11000000;
      break;
    case 9:
      PORTD |= B01011110;
      PORTB |= B10000000;
      break;
    case 11: // decimal point
      PORTD |= B00100000;
      break;
    default: // leave blank
      break;
  }
}
