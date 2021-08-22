#ifndef MotorController_h
#define MotorController_h

#ifndef Motor_h
  #include "Motor.h"
#endif

#define MOTORS_COUNT 1

const char Axes[] = { 'X' };

class MotorController {
  public:
    MotorController();
    Motor Motors[3];
    bool UseRamp;
    void Initialize();
    void Delete(Motor motor);
    void Move(float steps, Motor& motor);
    void MoveTo(float pos, Motor& motor);
    void RunToEnd(int8_t _direction);
    void CalculateRamp(float index, Motor& motor);
    void CalculateRamp(float delta, float index, Motor& motor);
};

#endif
