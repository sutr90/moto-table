#include <Arduino.h>

const int stepsPerRevolution = 2000 * 14;

const int STEP_DELAY_US = 60;


const int ENABLE_PIN = 4;
const int STEP_PIN = 5;
const int LDIR_PIN = 3;
const int RDIR_PIN = 2;


void clockwise() {
  digitalWrite(LDIR_PIN, LOW);
  digitalWrite(RDIR_PIN, HIGH);
}

void counterclock() {
  digitalWrite(LDIR_PIN, HIGH);
  digitalWrite(RDIR_PIN, LOW);
}

void turnMotor() {
  digitalWrite(ENABLE_PIN, LOW);

  for (int x = 0; x < stepsPerRevolution; x++)
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(STEP_DELAY_US);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(STEP_DELAY_US);
  }

  digitalWrite(ENABLE_PIN, HIGH);
}


void setup()
{
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(LDIR_PIN, OUTPUT);
  pinMode(RDIR_PIN, OUTPUT);
}
void loop()
{
  clockwise();
  turnMotor();
  delay(1000); // Wait a second

  counterclock();
  turnMotor();
  delay(1000); // Wait a second
}