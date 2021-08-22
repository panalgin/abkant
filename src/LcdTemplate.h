#ifndef LcdTemplate_h
#define LcdTemplate_h

#ifndef Arduino_h
  #include "Arduino.h"
#endif

#include <LiquidCrystal.h>


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
    void HookKey(char key, LiquidCrystal &lcd);
    void Print(LiquidCrystal &lcd);
    void printNumber(char key, LiquidCrystal &lcd);
    void deleteNumber(LiquidCrystal &lcd);
    void updateCursor(LiquidCrystal &lcd);
};

#endif

