#include <Arduino.h>
#include <EEPROM.h>
#include <OneButton.h>
#include <NewPing.h>

#define TRIGGER_PIN 8    // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN 7       // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define MAX_HEIGHT 150 // max allowed height in cm

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

unsigned int pingSpeed = 200; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;      // Holds the next ping time.

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

unsigned long currentHeight = 0;

unsigned int setHeight = 150;

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

void enable()
{
  Serial.println("enable");
  digitalWrite(ENABLE_PIN, LOW);
}

void disable()
{
  Serial.println("disable");
  digitalWrite(ENABLE_PIN, HIGH);
}

int readCurrentHeight()
{
  Serial.println("reading height");
  int height = sonar.ping_cm();

  Serial.print("current height");
  Serial.println(height);

  return height;
}


void buzzMotorsSaved()
{
  Serial.println("buzzing");
  delay(50);
  enable();

  dirUp();
  for (int j = 0; j < 4000; j++)
  {
    turnMotor(2);
  }

  delay(400);

  dirDown();
  for (int j = 0; j < 4000; j++)
  {
    turnMotor(4);
  }

  disable();
}

void buzzMotorsCleared()
{
  Serial.println("buzzing");
  delay(50);
  enable();

  for (int j = 0; j < 18000; j++)
  {
    turnMotor(4);
  }

  disable();
}


void updateHeight(int height)
{
  unsigned char isHeightSet = EEPROM.read(0x1);

  if (isHeightSet > 0)
  {
    setHeight = MAX_HEIGHT;

    EEPROM.write(0x0, 0xff & MAX_HEIGHT);
    EEPROM.write(0x1, 0x0);

    Serial.print("cleared value");
    buzzMotorsCleared();
  }
  else
  {
    Serial.println("saving current height");
    EEPROM.write(0x0, height & 0xff);
    EEPROM.write(0x1, 0x1);

    Serial.print("stored value ");
    Serial.println(EEPROM.read(0x0));
    buzzMotorsSaved();
  }
}

void onDoubleLongPress()
{
  if (!run)
  {
    int height = readCurrentHeight();
    updateHeight(height);
  }
}

void setup()
{
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(LDIR_PIN, OUTPUT);
  pinMode(RDIR_PIN, OUTPUT);
  pinMode(ENDSTOP_MIN, INPUT_PULLUP);

  setHeight = EEPROM.read(0x0);

  disable();

  btnUp.attachDoubleClick([]()
                          {
                            dirUp();
                            run = true;
                            down = false;
                            enable();
                            currentHeight = 0; });

  btnDown.attachDoubleClick([]()
                            {
                              dirDown();
                              run = true;
                              down = true;
                              enable(); });

  btnUp.attachClick([]()
                    { run = false; disable(); });

  btnDown.attachClick([]()
                      { run = false; disable(); });

  Serial.begin(9600);
  pingTimer = millis(); // Start now.

  Serial.println("MotoTable initialized");
  Serial.print("Current set height is ");
  Serial.println(setHeight);
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

void echoCheck()
{ // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer())
  {
    currentHeight = sonar.ping_result / US_ROUNDTRIP_CM;
    Serial.println(currentHeight);
  }
  // Don't do anything here!
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

  if (run && currentHeight < MAX_HEIGHT)
  {
    turnMotor();
  }

  if (run && !down && currentHeight > setHeight)
  {
    run = false;
    Serial.print("Reached set height ");
    Serial.print(setHeight);
    Serial.println(", stopping");
  }

  if (btnDown.isLongPressed() && btnUp.isLongPressed())
  {
    Serial.println("dbl long press");
    btnDown.reset();
    btnUp.reset();

    onDoubleLongPress();
  }

  if (millis() >= pingTimer && run && !down)
  {                              // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }
}