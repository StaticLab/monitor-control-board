/*
   MONITOR CONTROLLER V2

   Now uses AccelStepper library for acceleration

   Controls the orientation of a monitor using an DRV8825 or A4988 and a NEMA 17 stepper motor.
   Uses 2 limit switches to stop rotation at vertical & horizontal positions.
   Also utilizes a momentary switch to manually switch orientatians if necessary.

   ***********************
   CONNECTIONS:

   NEMA 17 Coils = Black/Green, Red/Blue

   ** FOR A4988 ONLY **
   To stop motor chatter on startup/reset, place a 10k pulldown resistor on
   the step pin (step pin to Arduino, plus step pin to 10k then to ground).
   (only needed for A4988 controller)

   All switches connect to ground and their respeccted pins.

********************************************************************************/

#include <AccelStepper.h>

// for debugging
const bool debug = false;

// defines DRV8825/A4988 pins numbers
const int dirPin = 2;
const int stepPin = 3;
const int mode0 = 6;
const int mode1 = 7;
const int mode2 = 8;
const int enablePin = 9;

// define switch pins
const int rotateButton = 10;
const int v_Limiter = 11;
const int h_Limiter = 12;

// motor settings
const int maxSpeed = 1000;
const int accelSpeed = 1000;
const int endPos = 2000;

// define mode and the pins
AccelStepper stepper(1, stepPin, dirPin); // driver, step, dir

// for current orientation
bool isHorizontal;

void setup()
{
  // init serial
  Serial.begin(9600);

  // init AccelStepper
  stepper.setEnablePin(enablePin);
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(accelSpeed);

  /***************************
   *  MICROSTEPS: M0 M1 M2
   *  FULL: LOW LOW LOW
   *  HALF: HIGH LOW LOW
   *  1/4: LOW HIGH LOW
   *  1/8: HIGH HIGH LOW
   *  1/16: LOW LOW HIGH
   *  1/32: HIGH LOW HIGH
   ***************************/

  // microstep @ 1/16th
  digitalWrite(mode0, LOW);
  digitalWrite(mode1, LOW);
  digitalWrite(mode2, HIGH);

  // switches/buttons
  pinMode(h_Limiter, INPUT_PULLUP);
  pinMode(v_Limiter, INPUT_PULLUP);
  pinMode(rotateButton, INPUT_PULLUP);

  // init monitor to home position (horizontal)
  Serial.println("setting to home position");
  setHorizontal();
}

void loop()
{
  if (debug) onDebug();

  // check if rotate button pressed
  if (digitalRead(rotateButton) == LOW)
  {
    Serial.println("Rotate Button Pressed...");
    isHorizontal ? setVertical() : setHorizontal();
  }
  
  // check if serial is available
  if (Serial.available() > 0)
  {
    // read the incoming string and trim the trailing \n
    String serialString = Serial.readString();
    serialString.trim();

    if (serialString == "H" && !isHorizontal)
    {
      Serial.println("setting to horizontal position");
      setHorizontal();
    }
    else if (serialString == "V" && isHorizontal)
    {
      Serial.println("setting to vertical position");
      setVertical();
    }
    else {
      Serial.println("monitor already in position");
    }
  }
}

void setHorizontal()
{
  stepper.disableOutputs();
  stepper.moveTo(endPos);

  // if h_Limiter is not pressed
  while (!digitalRead(h_Limiter) && stepper.distanceToGo() > 0)
  {
    stepper.run();
  }

  isHorizontal = true;
  stepper.enableOutputs();
  stepper.setCurrentPosition(0);
}

void setVertical()
{
  stepper.disableOutputs();
  stepper.moveTo(-endPos);

  // if v_Limiter is not pressed
  while (!digitalRead(v_Limiter) && stepper.distanceToGo() < 0)
  {
    stepper.run();
  }

  isHorizontal = false;
  stepper.enableOutputs();
  stepper.setCurrentPosition(0);
}

void onDebug()
{
  // print to console if either limit switch is pressed
  
  if (digitalRead (h_Limiter)) {
    Serial.println ("H Limit Pressed...");
  }
  if (digitalRead (v_Limiter)) {
    Serial.println ("V Limit Pressed...");
  }
}
