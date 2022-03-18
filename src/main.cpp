#include <Arduino.h>
#include <OneButton.h>

bool run = false;
bool down = false;

OneButton btnUp = OneButton(A1);
OneButton btnDown = OneButton(11);

const int STEP_DELAY_US = 60;

const int ENABLE_PIN = 4;
const int STEP_PIN = 9;
const int LDIR_PIN = 3;
const int RDIR_PIN = 5;

const int ENDSTOP_MIN = 12;

void dirUp()
{
  digitalWrite(LDIR_PIN, LOW);
  digitalWrite(RDIR_PIN, HIGH);
  Serial.println("up");
}

void dirDown()
{
  digitalWrite(LDIR_PIN, HIGH);
  digitalWrite(RDIR_PIN, LOW);
  Serial.println("down");
}

void turnMotor(int delay = STEP_DELAY_US)
{
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(delay);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(delay);
}

void enable() { Serial.println("enable"); digitalWrite(ENABLE_PIN, LOW); }

void disable() { Serial.println("disable"); digitalWrite(ENABLE_PIN, HIGH); }

void setup()
{
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(LDIR_PIN, OUTPUT);
  pinMode(RDIR_PIN, OUTPUT);
  pinMode(ENDSTOP_MIN, INPUT_PULLUP);

  disable();

  btnUp.attachDoubleClick([]()
                          {
                            dirUp();
                            run = true;
                            down = false;
                            enable();
                          });

  btnDown.attachDoubleClick([]()
                            {
                              dirDown();
                              run = true;
                              down = true;
                              enable();
                            });

  btnUp.attachClick([]()
                    { run = false; disable(); });

  btnDown.attachClick([]()
                      { run = false; disable(); });

  Serial.begin(9600);
}

void moveBack()
{
  delay(50);
  dirUp();
  for (int j = 0; j < 5000; j++)
  {
    turnMotor(STEP_DELAY_US * 2);
  }
}

void loop()
{
  btnUp.tick();
  btnDown.tick();

  if (digitalRead(ENDSTOP_MIN) == LOW)
  {
    run = false;
    moveBack();
  }

  if (run)
  {
    turnMotor();
  }
}