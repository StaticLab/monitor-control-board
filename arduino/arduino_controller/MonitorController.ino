/*
 * MONITOR CONTROLLER V1
 
   Controls the orientation of a monitor using an DRV8825 or A4988 and a NEMA 17 stepper.
   Uses 2 limit switches to stop rotation at vertical & horizontal positions.
   Also utilizes a momentary switch to manually switch orientatians.

   ***********************
   CONNECTIONS:

   NEMA 17 Coils = Black/Green, Red/Blue

   To stop motor chatter on startup/reset, place a 10k pulldown resistor on
   the step pin (step pin to Arduino, plus step pin to 10k then to ground).

   All switches connect to ground and their respeccted pins.

   ***********************
   WINDOWS/GAMEEX SETUP:

   


 ********************************************************************************/

// defines DRV8825/A4988 pins numbers
const int dirPin = 2;
const int stepPin = 3;
const int enablePin = 9;

// define switch pins
const int rotateButton = 10;
const int v_Limiter = 11;
const int h_Limiter = 12;

// motor speed
const int m_delay = 2000;
const int maxSpeed = 800; // less is faster
int currSpeed = m_delay;

// for current orientation
bool isHorizontal;

void setup()
{
  // initalize the serial port
  Serial.begin(9600);

  // disable motor control
  digitalWrite(enablePin, HIGH);

  // Sets the two pins as Outputs
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  digitalWrite(stepPin, LOW);

  // limit switch
  pinMode(h_Limiter, INPUT_PULLUP);
  pinMode(v_Limiter, INPUT_PULLUP);

  // user button
  pinMode(rotateButton, INPUT_PULLUP);

  // set monitor to home position (horizontal)
  delay(1000);
  Serial.println("setting to home position");
  
  setHorizontal();
}

void loop()
{
  // check if reset button pressed
  if (digitalRead(rotateButton) == LOW)
  {
    Serial.println("user button pressed");
    if (isHorizontal) setVertical();
    else if (!isHorizontal) setHorizontal();
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
  digitalWrite(enablePin, LOW);
  digitalWrite(dirPin, HIGH); // counter-clockwise

  // if h_Limiter is not pressed
  while (!digitalRead(h_Limiter))
  {
    rotateMonitor();
  }

  isHorizontal = true;
  currSpeed = m_delay;
  digitalWrite(enablePin, HIGH);
}

void setVertical()
{
  digitalWrite(enablePin, LOW);
  digitalWrite(dirPin, LOW); // clockwise

  // if v_Limiter is not pressed
  while (!digitalRead(v_Limiter))
  {
    rotateMonitor();
  }

  isHorizontal = false;
  currSpeed = m_delay;
  digitalWrite(enablePin, HIGH);
}

void rotateMonitor()
{
  // use acceleration
  if (currSpeed > maxSpeed) currSpeed -= 10;
  
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(currSpeed);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(currSpeed);
}
