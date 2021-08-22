#include "Motor.h"

Motor::Motor() {
  
}

Motor::Motor(uint8_t steppin, uint8_t dirpin, char axis) {
  this->StepPin = steppin;
  this->DirPin = dirpin;
  this->Axis = axis;

}

void Motor::SetDirection() {
  if (this->Direction == 1)
    digitalWrite(this->DirPin, this->IsInverted ? LOW : HIGH);
  else
    digitalWrite(this->DirPin, this->IsInverted ? HIGH : LOW);
}

void Motor::ChangeDirection(int8_t dir) {
  if (this->Direction != dir) {
    this->Direction = dir; 
    this->SetDirection();
  }
}

void Motor::Initialize() {
  this->StepsPerRevolution = 200;
  this->BaseMetricInSteps = 40.00f; //1mm = 40.002366f steps

  this->SetSpeed(60);
  this->Direction = -1;
  
  this->MicroStepRatio = 16;
  this->CurrentPosition = 0;
  this->TargetPosition = 0;
  this->StepsRemaining = 0;
  this->StepsTaken = 0;
  this->LastSteppedAt = micros() - (this->StepInterval * 2); // make it ready to step instantly on first loop cycle
  this->UseRamping = true;
  
  pinMode(this->StepPin, OUTPUT);
  pinMode(this->DirPin, OUTPUT);
  
  if (this->MinPin > 0)
    pinMode(this->MinPin, INPUT);
  
  digitalWrite(this->StepPin, LOW);
  digitalWrite(this->DirPin, LOW);
}

void Motor::Prepare(float relativeTarget) {
  this->TargetPosition = this->CurrentPosition + relativeTarget;
  this->StepsRemaining = abs(this->TargetPosition - this->CurrentPosition);
  this->StepsTaken = 0;
  
  if (this->TargetPosition < this->CurrentPosition)
    this->Direction = -1;
  else
    this->Direction = 1;
    
  this->SetDirection();
}

void Motor::PrepareTo(float absoluteTarget) {
  this->TargetPosition = absoluteTarget;
  this->StepsRemaining = abs(this->TargetPosition - this->CurrentPosition);
  this->StepsTaken = 0;
  
  if (this->TargetPosition < this->CurrentPosition)
    this->Direction = -1;
  else
    this->Direction = 1;
    
  this->SetDirection();
}

void Motor::PrepareConstant(int8_t dir) {
  this->TargetPosition = 0;
  this->StepsRemaining = 0;
  this->StepsTaken = 0;
 
  this->Direction = dir;
  this->SetDirection();
}

void Motor::Stop() {
  this->StepsRemaining = 0;
  this->StepsTaken = 0;
}

void Motor::Move(float relativeTarget) {
  this->Prepare(relativeTarget);
  
  while(this->StepsRemaining > 0) {
    this->Step();
  }
}

void Motor::MoveTo(float absoluteTarget) {
  this->PrepareTo(absoluteTarget);
  
  while(this->StepsRemaining >= 1) {
    this->Step();
  }
}

void Motor::Step() {
   while(true) {
     if (micros() - this->LastSteppedAt >= this->StepInterval) {
       this->LastSteppedAt = micros();
       
       digitalWrite(this->StepPin, HIGH);
       this->StepsTaken++;
       delayMicroseconds(1);
       digitalWrite(this->StepPin, LOW);
       this->CurrentPosition += this->Direction;
       this->StepsRemaining--;
       
       break;
     }
  }
}

void Motor::JoyStep() {
  if (micros() - this->LastSteppedAt >= this->StepInterval) {
    this->LastSteppedAt = micros();
       
    digitalWrite(this->StepPin, HIGH);
    delayMicroseconds(1);
    digitalWrite(this->StepPin, LOW);
    this->CurrentPosition += this->Direction;
  }
}

float Motor::GetCurrentPosition() {
  return this->CurrentPosition;
}

void Motor::SetCurrentPosition(float pos) {
   this->CurrentPosition = pos;
}

float Motor::GetSpeed() {
  return this->Speed;
}

void Motor::SetSpeed(float whatSpeed) {
  if (this->Speed != whatSpeed) {
    this->StepInterval = 60000000 / (this->StepsPerRevolution * this->MicroStepRatio) / whatSpeed;
    this->Speed = whatSpeed;
  }
}

void Motor::SetDwellSpeed(float whatSpeed) {
  if (this->DwellSpeed != whatSpeed)
    this->DwellSpeed = whatSpeed;
}

void Motor::SetMaxSpeed(float whatSpeed) {
  if (this->MaxSpeed != whatSpeed) 
    this->MaxSpeed = whatSpeed;
}

void Motor::RampDown() {
  unsigned long rampIndex = (unsigned long)(this->BaseMetricInSteps * 30.0f); //30mm mesafede yavaşla
  unsigned long currentRpm = 1;
  
  while(rampIndex > 0) {
      rampIndex--;

      if (rampIndex % 5 == 0) {
      
        currentRpm = max(1, map(rampIndex, 0, 1201, 0, 350));
  
        if (this->Speed >= currentRpm) // prevent slow to high brake
          this->SetSpeed(currentRpm); 
      }
      
      this->Step();
  }
}

void Motor::Offset() {
  int SwitchState = digitalRead(this->MinPin);
  this->ChangeDirection(1);
  this->SetSpeed(this->DwellSpeed);
  
  while(SwitchState == LOW) {
    this->Step();
    
    SwitchState = digitalRead(this->MinPin);
  }
}
