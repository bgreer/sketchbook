
/* ironman.ino
	Pulses some lights based on button input.
*/

#define CHEST_LED 8
#define HAND_LED 7
#define BUTTON 1

int val, dir, minval, maxval, handval;
int pause, i;
float heartrate;

void setup()  {
	// max and min brightness
  maxval = 200;
  minval = 50;
  val = minval;
	// direction of the pulse, positive or negative
  dir = 1;
  heartrate = 0.3;
	// delay depends on heartrate and how many values to go through
  pause = (int)(1000./(heartrate*2.*(maxval-minval)));
  handval = minval;
	// set pinmodes
  pinMode(CHEST_LED, OUTPUT);
  pinMode(HAND_LED, OUTPUT);
  analogWrite(CHEST_LED, minval);
  analogWrite(HAND_LED, handval);
  pinMode(BUTTON, INPUT);
} 

void loop()  { 
  // check for fire because attiny hates being interrupted
	// but only allow firing when heartrate is low enough
  if (digitalRead(BUTTON) == HIGH && heartrate <= 0.3)
  {
    // fade chest
    while(val > 1)
    {
      val -= 1;
      analogWrite(CHEST_LED, val);
      delay(1);
    }
    // light up hand
    while(handval < 255)
    {
      handval += 5;
      analogWrite(HAND_LED, handval);
      delay(1);
    }
		// strobe hand a bit
    for (i=0; i<50; i++)
    {
      analogWrite(HAND_LED, 255);
      delay(20);
      analogWrite(HAND_LED, 100);
      delay(20);
    }
		// return hand to low value
    analogWrite(HAND_LED, minval);
    val = minval;
    dir = 1;
		// speed up heartrate
    heartrate = 2.0;
    pause = (int)(1000./(heartrate*2.*(maxval-minval)));
  }
  
	// every time through the loop, write the chest value
  analogWrite(CHEST_LED, val);
	// then increment it
  val += dir;
	// check for hitting the boundaries
  if (val < minval || val > 255)
  {
    val -= dir;
    dir *= -1;
		// if heartrate is elevated, slow it down a bit
    if (heartrate > 0.3) {
      heartrate -= 0.1;
      pause = (int)(1000./(heartrate*2.*(maxval-minval)));
    }
  }
  delay(pause);
}
