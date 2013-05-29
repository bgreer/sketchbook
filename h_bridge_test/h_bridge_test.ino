#define ENABLE 10
#define A 9
#define B 8

int dir= 0;

void setup()
{
  pinMode(ENABLE, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(ENABLE, HIGH);
}

void loop()
{
  if (dir==0)
  {
    digitalWrite(A, LOW);
    digitalWrite(B, HIGH);
    dir = 1;
  } else {
    digitalWrite(A, HIGH);
    digitalWrite(B, LOW);
    dir = 0;
  }
  delay(500);
}
