#include <Arduino.h>
// include the library code:
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <string.h>
#include "LcdTemplate.h"
#include "MotorController.h"

#include <Keypad_I2C.h>
#include <Keypad.h>          // GDY120705
#include <Wire.h>

#define I2CADDR 0x27

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
const char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = { 3, 0, 7, 6 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 5, 4, 2, 1 }; //connect to the column pinouts of the keypad

Keypad_I2C keypad = Keypad_I2C( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR );

LiquidCrystal lcd(9, 10, 11, 12, 13, 8, A1, A2, A3, 2);

MotorController m_Controller;
Motor x_Motor(0, 1, 'X');

int backOffSensor = 3;

LcdTemplate m_Splash(0);
LcdTemplate m_Main(1);

const uint8_t pressureSensorPin = A0;
const uint8_t pressureRelayPin = 7;
const uint8_t pressureModePin = 6;

void setup() {
  //Serial.begin(9600);
  pinMode(backOffSensor, INPUT);
  pinMode(pressureSensorPin, INPUT);
  pinMode(pressureRelayPin, OUTPUT);
  pinMode(pressureModePin, INPUT);

  digitalWrite(pressureRelayPin, LOW);

  lcd.begin(20, 4);

  m_Splash.Print(lcd);
  delay(2000);
  m_Splash.Type = 5;
  m_Splash.Deter();
  m_Splash.Print(lcd);

  x_Motor.Initialize();

  x_Motor.IsInverted = false;
  x_Motor.MinPin = 4;


  x_Motor.MicroStepRatio = 16;
  x_Motor.SetMaxSpeed(275);
  x_Motor.SetDwellSpeed(20);

  x_Motor.ShortDistance = x_Motor.BaseMetricInSteps * 30.0;
  x_Motor.RampStartsAt = 15;

  x_Motor.SetSpeed(275);

  m_Controller.Initialize();
  m_Controller.Motors[0] = x_Motor;

  m_Controller.Move((x_Motor.BaseMetricInSteps * 60.0f), x_Motor);

  /*m_Controller.MoveTo(-80000, x_Motor);
    delay(2000);
    m_Controller.MoveTo(80000, x_Motor);

    delay(5000);*/

  int8_t _direction = -1;
  m_Controller.RunToEnd(_direction);

  calibreSiper();

  m_Main.Print(lcd);

  keypad.begin( );
  keypad.addEventListener(keypadEvent); //add an event listener for this keypad

  // TIMER 1 for interrupt frequency 50 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 50 Hz increments
  OCR1A = 39999; // = 16000000 / (8 * 50) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 8 prescaler
  TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts
}

uint8_t backOffSensorState = HIGH;

bool backedOff = false;
bool sensorPressed = false;

unsigned long lastTransmittedAt = 0;
unsigned long lastPressuredAt = 0;
bool hasPressureRelieved = false;

#define MAX_PRESSURE_VOLTAGE 650

ISR(TIMER1_COMPA_vect) {
  //if (backOffSensorState == LOW && backedOff == true)  {

  //if (millis() - lastPrintedAt > 100) {
  //lastPrintedAt = millis();
  uint16_t pressureValue = analogRead(pressureSensorPin);
  uint16_t pressureInBar = map(pressureValue, 0, 1023, 0, 400);

  if (pressureValue >= MAX_PRESSURE_VOLTAGE) {
    if (hasPressureRelieved == false) {
      hasPressureRelieved = true;
      lastPressuredAt = millis();
    }
  }

  if (hasPressureRelieved) {
    unsigned long timeElapsed = millis() - lastPressuredAt;
    uint16_t modePenalty = digitalRead(pressureModePin) == HIGH ? 550 : 0;
    
    if (timeElapsed > 350 + modePenalty)
    {
      if (timeElapsed < 3350 + modePenalty)
        digitalWrite(pressureRelayPin, HIGH);
      else {
        digitalWrite(pressureRelayPin, LOW);
        hasPressureRelieved = false;
      }
    }
  }

  //}

  //}
}


void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case RELEASED:
      m_Main.HookKey(key, lcd);
      break;
  }
}


void loop() {
  char key = keypad.getKey();

  backOffSensorState = digitalRead(backOffSensor);

  if (backOffSensorState == LOW && backedOff == false && sensorPressed == false) {
    delay(5);

    if (backOffSensorState == LOW) {
      sensorPressed = true;
      x_Motor.SetDwellSpeed(5);
      m_Controller.Move((3.0f * x_Motor.BaseMetricInSteps), x_Motor); //3mm geriye çık
      backedOff = true;
      x_Motor.SetDwellSpeed(50);
    }
  }
  else if (backOffSensorState == HIGH && sensorPressed == true && backedOff == true) {
    if (backOffSensorState == HIGH && sensorPressed == true && backedOff == true ) {
      x_Motor.SetDwellSpeed(5);
      m_Controller.Move(-(3.0f * x_Motor.BaseMetricInSteps), x_Motor); //3mm geriye gel
      sensorPressed = false;
      backedOff = false;
      x_Motor.SetDwellSpeed(50);
    }
  }

  if (m_Main.Type == 6) {
    if (m_Main.HasFiredMotor == false) {
      m_Main.HasFiredMotor = true;

      float distanceInUM = atof(m_Main.Out); // burayı kontrol et

      float finalDestination = distanceInUM * (x_Motor.BaseMetricInSteps / 10);
      float minimumDestination = x_Motor.BaseMetricInSteps * 6.5f;

      if (finalDestination >= minimumDestination) {
        x_Motor.SetMaxSpeed(120);
        x_Motor.SetDwellSpeed(25);
        x_Motor.SetSpeed(120);

        m_Controller.MoveTo(finalDestination, x_Motor); // NOT 40.0024 because 1mm = 40.0024 step 0.1mm = 4.00024 step

        m_Main.Type = 2;
        m_Main.Deter();
        m_Main.Print(lcd);
      }
    }
  }
  else if (m_Main.Type == 7) {
    calibreSiper();

    delay(1000);

    lcd.noBlink();
    lcd.noCursor();

    m_Main.Type = 1;
    m_Main.Deter();
    m_Main.Print(lcd);

  }
}

void calibreSiper() {
  uint8_t first = EEPROM.read(0);
  uint8_t second = EEPROM.read(1);
  uint8_t third = EEPROM.read(2);
  uint8_t fourth = EEPROM.read(3);

  char m_Buffer[4] = { first + 48, second + 48, third + 48, fourth + 48 }; //charları digite cevir

  float defaultDistanceInUN = atof(m_Buffer) * (x_Motor.BaseMetricInSteps / 100.0f);

  x_Motor.SetCurrentPosition(defaultDistanceInUN); //mm
  x_Motor.SetMaxSpeed(350);
  x_Motor.SetSpeed(350);
}

