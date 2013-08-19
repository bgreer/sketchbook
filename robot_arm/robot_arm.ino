#include <MatrixMath.h>

#define MOTOR0UPON {PORTB |= B00010000;}
#define MOTOR0UPOFF {PORTB &= B11101111;}
#define MOTOR0DOWNON {PORTB |= B00100000;}
#define MOTOR0DOWNOFF {PORTB &= B11011111;}

#define MOTOR1UPON {PORTB |= B00000100;}
#define MOTOR1UPOFF {PORTB &= B11111011;}
#define MOTOR1DOWNON {PORTB |= B00001000;}
#define MOTOR1DOWNOFF {PORTB &= B11110111;}

#define MOTOR2UPON {PORTD |= B10000000;}
#define MOTOR2UPOFF {PORTD &= B01111111;}
#define MOTOR2DOWNON {PORTB |= B00000001;}
#define MOTOR2DOWNOFF {PORTB &= B11111110;}

#define MOTOR3UPON {PORTD |= B00100000;}
#define MOTOR3UPOFF {PORTD &= B11011111;}
#define MOTOR3DOWNON {PORTD |= B01000000;}
#define MOTOR3DOWNOFF {PORTD &= B10111111;}

#define MOTOR4UPON {PORTD |= B00010000;}
#define MOTOR4UPOFF {PORTD &= B11101111;}
#define MOTOR4DOWNON {PORTB |= B00000010;}
#define MOTOR4DOWNOFF {PORTB &= B11111101;}

uint8_t motor_pin_up[5] =   {12, 10, 7, 5, 4};
uint8_t motor_pin_down[5] = {13, 11, 8, 6, 9};
uint8_t angle_sensor[4] = {0, 1, 2, 3};
int currspeed[5] = {0,0,0,0,0};
uint8_t counter = 0;

float A[4][4];

float clawX, clawY, clawZ;

int ii;
int ij;
int it;

void setup ()
{
  Serial.begin(38400);
  // speed up analog read
  bitClear(ADCSRA,ADPS0);
  bitClear(ADCSRA,ADPS1);
  bitSet(ADCSRA,ADPS2);
  
  // set pinmodes
  for (ii=0; ii<5; ii++)
  {
    pinMode(motor_pin_up[ii], OUTPUT);
    pinMode(motor_pin_down[ii], OUTPUT);
    digitalWrite(motor_pin_up[ii], LOW);
    digitalWrite(motor_pin_down[ii], LOW);
  }
  TCCR1B=1; //set no prescaler
  TIMSK1=(1<<TOIE1); //enable timer1 over-flow interrupt
  sei(); //enable global interrupts
}

ISR (TIMER1_OVF_vect)
{
  counter ++;
  
  if (counter == 0)
  {
    // reset, turn motors on if needed
    if (currspeed[0] > 20)
      MOTOR0UPON
    else if (currspeed[0] < -20)
      MOTOR0DOWNON
    if (currspeed[1] > 20)
      MOTOR1UPON
    else if (currspeed[1] < -20)
      MOTOR1DOWNON
    if (currspeed[2] > 20)
      MOTOR2UPON
    else if (currspeed[2] < -20)
      MOTOR2DOWNON
    if (currspeed[3] > 20)
      MOTOR3UPON
    else if (currspeed[3] < -20)
      MOTOR3DOWNON
    if (currspeed[4] > 20)
      MOTOR4UPON
    else if (currspeed[4] < -20)
      MOTOR4DOWNON
  } else {
    // does a motor need to be stopped for a moment?
    if (counter > abs(currspeed[0]))
    {
      MOTOR0UPOFF;
      MOTOR0DOWNOFF;
    }
    if (counter > abs(currspeed[1]))
    {
      MOTOR1UPOFF;
      MOTOR1DOWNOFF;
    }
    if (counter > abs(currspeed[2]))
    {
      MOTOR2UPOFF;
      MOTOR2DOWNOFF;
    }
    if (counter > abs(currspeed[3]))
    {
      MOTOR3UPOFF;
      MOTOR3DOWNOFF;
    }
    if (counter > abs(currspeed[4]))
    {
      MOTOR4UPOFF;
      MOTOR4DOWNOFF;
    }
  }
}

void loop ()
{
  //currspeed[0] = (int)(250.*sin(millis()/1000.));
  currspeed[0] = (int)(20*(0.0-getAngle(0)));
  currspeed[1] = (int)(20*(90.0-getAngle(1)));
  currspeed[2] = (int)(20*(0.0-getAngle(2)));
  currspeed[3] = (int)(20*(0.0-getAngle(3)));
  currspeed[4] = 100;
  
  for (ii=0; ii<4; ii++)
    if (abs(currspeed[ii]) < 50) currspeed[ii] = 0;
    
  //checkBounds();
  
  computePos();
  /*
  Serial.print(clawX);
  Serial.print("\t");
  Serial.print(clawY);
  Serial.print("\t");
  Serial.println(clawZ);
  delay(10);
  */
  Serial.println("Inverting matrix..");
  invert((float*)A,4);
  Serial.println("Done.");
}


float accurateRead(int pin)
{
  float temp = 0.0;
  
  for (ii=0; ii<4; ii++)
    analogRead(pin);
  
  for (ii=0; ii<30; ii++)
    temp += analogRead(pin);
  
  return temp / 30.0;
}



// method taken from MatrixMath library by Charlie Matlack
int invert(float* A, int n)
{
	// A = input matrix AND result matrix
	// n = number of rows = number of columns in A (n x n)
	int pivrow;		// keeps track of current pivot row
	int k,i,j;		// k: overall index along diagonal; i: row index; j: col index
	int pivrows[n]; // keeps track of rows swaps to undo at end
	float tmp;		// used for finding max value and making column swaps

	for (k = 0; k < n; k++)
	{
		// find pivot row, the row with biggest entry in current column
		tmp = 0;
		for (i = k; i < n; i++)
		{
			if (abs(A[i*n+k]) >= tmp)	// 'Avoid using other functions inside abs()?'
			{
				tmp = abs(A[i*n+k]);
				pivrow = i;
			}
		}

		// check for singular matrix
		if (A[pivrow*n+k] == 0.0f)
		{
			Serial.println("Inversion failed due to singular matrix");
			return 0;
		}

		// Execute pivot (row swap) if needed
		if (pivrow != k)
		{
			// swap row k with pivrow
			for (j = 0; j < n; j++)
			{
				tmp = A[k*n+j];
				A[k*n+j] = A[pivrow*n+j];
				A[pivrow*n+j] = tmp;
			}
		}
		pivrows[k] = pivrow;	// record row swap (even if no swap happened)

		tmp = 1.0f/A[k*n+k];	// invert pivot element
		A[k*n+k] = 1.0f;		// This element of input matrix becomes result matrix

		// Perform row reduction (divide every element by pivot)
		for (j = 0; j < n; j++)
		{
			A[k*n+j] = A[k*n+j]*tmp;
		}

		// Now eliminate all other entries in this column
		for (i = 0; i < n; i++)
		{
			if (i != k)
			{
				tmp = A[i*n+k];
				A[i*n+k] = 0.0f;  // The other place where in matrix becomes result mat
				for (j = 0; j < n; j++)
				{
					A[i*n+j] = A[i*n+j] - A[k*n+j]*tmp;
				}
			}
		}
	}

	// Done, now need to undo pivot row swaps by doing column swaps in reverse order
	for (k = n-1; k >= 0; k--)
	{
		if (pivrows[k] != k)
		{
			for (i = 0; i < n; i++)
			{
				tmp = A[i*n+k];
				A[i*n+k] = A[i*n+pivrows[k]];
				A[i*n+pivrows[k]] = tmp;
			}
		}
	}
	return 1;
}
