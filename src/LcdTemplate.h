#ifndef LcdTemplate_h
#define LcdTemplate_h

#ifndef Arduino_h
  #include "Arduino.h"
#endif

#include <LiquidCrystal_I2C.h>


class LcdTemplate { 
  private:
    
    
  public:
    LcdTemplate();
    LcdTemplate(short type);
    
    
    short Type;
    char Data[4][20];
    char LastKey;
    bool IsNewKey;
    bool IsVisible;
    bool HasFiredMotor;
    char Out[4];
    
    void Deter();
    void HookKey(char key, LiquidCrystal_I2C* lcd);
    void Print(LiquidCrystal_I2C* lcd);
    void printNumber(char key, LiquidCrystal_I2C* lcd);
    void deleteNumber(LiquidCrystal_I2C* lcd);
    void updateCursor(LiquidCrystal_I2C* lcd);
};

#endif
