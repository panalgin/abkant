#include "MotorController.h"

MotorController::MotorController() {

}

void MotorController::Initialize() {
  this->UseRamp = true;
}

void MotorController::Move(float steps, Motor& motor) {
  float absolute = motor.GetCurrentPosition() + steps;
  this->MoveTo(absolute, motor);
}

void MotorController::MoveTo(float absolute, Motor& motor) {
  float curPos = motor.GetCurrentPosition();

  if (curPos != absolute) {
    float delta = abs(curPos - absolute);
    motor.PrepareTo(absolute);

    float accelIndex = 0;

    while (motor.StepsRemaining >= 1) {
      if (motor.UseRamping)
        this->CalculateRamp(delta, accelIndex, motor);

      motor.Step();
      accelIndex++;
    }

    motor.SetSpeed(motor.MaxSpeed);
  }
}

void MotorController::RunToEnd(int8_t _direction) {
  uint8_t x_Index = 0;

  for (uint8_t i = 0; i < MOTORS_COUNT; i++) {
    if (this->Motors[i].Axis == 'X')
      x_Index = i;
  }

  Motor & x_Motor = this->Motors[x_Index];

  x_Motor.PrepareConstant(_direction);

  uint8_t xSwitchState = LOW;

  bool x_Offsetted = false;

  unsigned long x_AccelIndex = 0;

  xSwitchState = digitalRead(x_Motor.MinPin);

  if (xSwitchState == HIGH) {
    while (x_Offsetted == false) {
      if (x_Offsetted == false) {
        if (xSwitchState == HIGH) {
          xSwitchState = digitalRead(x_Motor.MinPin);
          this->CalculateRamp(x_AccelIndex, x_Motor);
          x_Motor.Step();
          x_AccelIndex++;
        }
        else {
          x_Offsetted = true;
          x_Motor.RampDown();
        }
      }
    }
  }

  x_Motor.Offset();
  x_Motor.SetCurrentPosition(6600);
}

void MotorController::CalculateRamp(float index, Motor& motor) {
  this->CalculateRamp(50000000, index, motor);
}

void MotorController::CalculateRamp(float delta, float index, Motor& motor) {
  //unsigned long delta = _delta;
  //unsigned long index = _index;

  if (this->UseRamp) {
    float currentRpm = 1.0;
    float accelerationEndsAt = motor.ShortDistance / 2; //(long)(m_DeltaY * 0.10);
    float decelerationStartsAt = delta - (motor.ShortDistance / 2); //(long)(m_DeltaY * 0.90);
    int maxRpm = max(1, motor.MaxSpeed);
    int minRpm = max(1, motor.RampStartsAt);

    if (delta < motor.ShortDistance) {
      if (index <= 0) { // performance reasons
        motor.SetSpeed(motor.DwellSpeed);
      }
    }
    else {
      if ((unsigned long)index % 5 == 0) {
        if (index <= accelerationEndsAt || index >= decelerationStartsAt) {
          if (index <= accelerationEndsAt) {
            currentRpm = max(1, minRpm + ((maxRpm - minRpm) * (index / accelerationEndsAt)));
            motor.SetSpeed(currentRpm);
          }
          else if (index >= decelerationStartsAt) {
            currentRpm = max(1, minRpm + ((maxRpm - minRpm) * (1.0f - (index / delta)) / (1.0f - (decelerationStartsAt / delta))));
            motor.SetSpeed(currentRpm);
          }
          else
            motor.SetSpeed(maxRpm);
        }
      }
    }
  }
}
