#ifndef Motor_h
#define Motor_h

#ifndef Arduino_h
  #include "Arduino.h"
#endif


class Motor { 
  private:
    uint8_t StepPin;
    uint8_t DirPin;
    uint8_t EnablePin;

    
    int StepsPerRevolution;
    unsigned long LastSteppedAt;
    
    void SetDirection();
    
  public:
    Motor();
    Motor(uint8_t steppin, uint8_t dirpin, char axis);
    
    uint8_t MinPin;
    uint8_t MaxPin;

    float BaseMetricInSteps;
    float MicroStepRatio;
    float CurrentPosition;
    float TargetPosition;
    float StepsRemaining;
    float StepsTaken;
    float StepInterval;
    int8_t Direction;
    bool IsInverted;
    bool UseRamping;
    
    float DwellSpeed;
    float Speed;
    float MaxSpeed;
    
    float ShortDistance;
    int RampStartsAt;
    
    
    char Axis;
    
    void Initialize();
    void Prepare(float relativeTarget);
    void PrepareTo(float absoluteTarget);
    void PrepareConstant(int8_t dir);
    
    void Move(float relativeTarget);
    void MoveTo(float absoluteTarget);
    void Step();
    void JoyStep();
    void SetMaxSpeed(float whatSpeed);
    void SetDwellSpeed(float whatSpeed);
    void SetSpeed(float whatSpeed);
    float GetSpeed();
    float GetCurrentPosition();
    void SetCurrentPosition(float pos);
    void Stop();
    void ChangeDirection(int8_t dir);
    void RampDown();
    void Offset();
};

#endif
