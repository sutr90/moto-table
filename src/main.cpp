#include <Arduino.h>
#include <OneButton.h>

bool run = false;

OneButton btnUp = OneButton(10);
OneButton btnDown = OneButton(11);

const int STEP_DELAY_US = 60;

const int ENABLE_PIN = 4;
const int STEP_PIN = 9;
const int LDIR_PIN = 3;
const int RDIR_PIN = 2;

void clockwise()
{
  digitalWrite(LDIR_PIN, LOW);
  digitalWrite(RDIR_PIN, HIGH);
}

void counterclock()
{
  digitalWrite(LDIR_PIN, HIGH);
  digitalWrite(RDIR_PIN, LOW);
}

void enable()
{
  digitalWrite(ENABLE_PIN, HIGH);
}

void disable()
{
  digitalWrite(ENABLE_PIN, LOW);
}

void turnMotor()
{
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(STEP_DELAY_US);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(STEP_DELAY_US);
}

void setup()
{
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(LDIR_PIN, OUTPUT);
  pinMode(RDIR_PIN, OUTPUT);

  btnUp.attachDoubleClick([]()
                             {
                               clockwise();
                               run = true;
                             });

  btnDown.attachDoubleClick([]()
                               {
                                 counterclock();
                                 run = true;
                               });

  btnUp.attachClick([]()
                            { run = false; });

  btnDown.attachClick([]()
                              { run = false; });
}
void loop()
{
  btnUp.tick();
  btnDown.tick();

  if (run)
  {
    turnMotor();
  }
}