#include "LcdTemplate.h"
#include <EEPROM.h>

LcdTemplate::LcdTemplate() {

}

void LcdTemplate::Deter() {
  switch (this->Type) {
    case 0:
      {
        sprintf(Data[0], "%s", "< KARA ELEKTRONIK  >");
        sprintf(Data[1], "%s", "  DAFLAN SOFTWARE   ");
        sprintf(Data[2], "%s", " ABKANT PRESS BRAKE ");
        sprintf(Data[3], "%s", "<ver: 0.3 bu: 07/17>");

        break;
      }
    case 1:
      {
        sprintf(Data[0], "%s", "A: OTOMATIK KULLANIM");
        sprintf(Data[1], "%s", "B: MANUEL KULLANIM  ");
        sprintf(Data[2], "%s", "C: AYARLAR          ");
        sprintf(Data[3], "%s", "                    ");

        break;
      }

    case 2:
      {
        sprintf(Data[0], "%s", "   -MANUEL PRESS-   ");
        sprintf(Data[1], "%s", "MESAFE:    00cm 00um");
        sprintf(Data[2], "%s", "              C: SIL");
        sprintf(Data[3], "%s", "*: GERI       D:  OK");

        break;
      }

    case 3:
      {
        sprintf(Data[0], "%s", "MODEL:      <KR-500>");
        sprintf(Data[1], "%s", "ASAMA:          9/11");
        sprintf(Data[2], "%s", "MESAFE:     000,00mm");
        sprintf(Data[3], "%s", "*: GERI     D: ILERI");

        break;
      }

    case 4:
      {
        uint8_t first = EEPROM.read(0);
        uint8_t second = EEPROM.read(1);
        uint8_t third = EEPROM.read(2);
        uint8_t fourth = EEPROM.read(3);

        sprintf(Data[0], "SIPER UZK:   %u%u.%u%umm", first, second, third, fourth);
        sprintf(Data[1], "%s", "MICROSTEP:        16");
        sprintf(Data[2], "%s", "1mm =       800 adim");
        sprintf(Data[3], "%s", "*: GERI    D: KAYDET");

        break;
      }

    case 5:
      {
        sprintf(Data[0], "%s", "                    ");
        sprintf(Data[1], "%s", "     KALIBRASYON    ");
        sprintf(Data[2], "%s", "   LUTFEN BEKLEYIN  ");
        sprintf(Data[3], "%s", "                    ");


        break;
      }

    case 6:
      {
        sprintf(Data[0], "%s", "                    ");
        sprintf(Data[1], "%s", "    ARKA DAYAMA     ");
        sprintf(Data[2], "%s", "    AYARLANIYOR     ");
        sprintf(Data[3], "%s", "                    ");

        break;
      }

    case 7:
      {
        sprintf(Data[0], "%s", "                    ");
        sprintf(Data[1], "%s", "      AYARLAR       ");
        sprintf(Data[2], "%s", "    KAYDEDILIYOR    ");
        sprintf(Data[3], "%s", "                    ");

        break;
      }
    case 8:
      {
        sprintf(Data[0], "%s", "   YETKILI GIRISI   ");
        sprintf(Data[1], "%s", "PIN:                ");
        sprintf(Data[2], "%s", "               C:SIL");
        sprintf(Data[3], "%s", "*:GERI         D: OK");

        break;
      }
  }
}

LcdTemplate::LcdTemplate(short type) {
  this->Type = type;
  this->Deter();
}

short numberCol = 0;

void LcdTemplate::Print(LiquidCrystal &lcd) {
  this->IsVisible = true;

  lcd.clear();

  for (short i = 0; i < 4; i++) {
    lcd.setCursor(0, i);

    for (short j = 0; j < 20; j++) {
      char cur = Data[i][j];

      if (cur > 0) {
        lcd.write(Data[i][j]);
        delay(3);
      }

      delay(1);
    }
  }

  switch (this->Type) {
    case 2:
      { // Manuel Ekran
        this->Out[0] = '0';
        this->Out[1] = '0';
        this->Out[2] = '0';
        this->Out[3] = '0';

        numberCol = 0;
        this->updateCursor(lcd);

        break;
      }
    case 4:
      { // Settings Ekranı
        this->Out[0] = '0';
        this->Out[1] = '0';
        this->Out[2] = '0';
        this->Out[3] = '0';

        numberCol = 0;
        this->updateCursor(lcd);

        break;
      }

    case 8:
      { // PIN Ekranı
        this->Out[0] = '0';
        this->Out[1] = '0';
        this->Out[2] = '0';
        this->Out[3] = '0';

        numberCol = 0;
        this->updateCursor(lcd);

        break;
      }
  }
}

void LcdTemplate::printNumber(char key, LiquidCrystal &lcd) {
  switch (this->Type) {
    case 2:
      { // Manuel Ekran
        lcd.write(key);
        this->Out[numberCol] = key;
        numberCol++;

        this->updateCursor(lcd);

        break;
      }
    case 4:
      { // Settings Ekranı
        lcd.write(key);
        this->Out[numberCol] = key;
        numberCol++;

        this->updateCursor(lcd);

        break;

      }
    case 8:
      {
        // PIN Ekranı
        lcd.write('*');
        this->Out[numberCol] = key;
        numberCol++;
        this->updateCursor(lcd);

        break;
      }
  }
}

void LcdTemplate::updateCursor(LiquidCrystal &lcd) {
  switch (this->Type) {
    case 2:
      { // Manuel Ekran
        if (numberCol > 3 || numberCol < 0)
          numberCol = 0;

        lcd.setCursor(11 + (numberCol > 1 ? numberCol + 3 : numberCol), 1);
        lcd.blink();

        break;
      }
    case 4:
      { // Settings Ekran
        if (numberCol > 3 || numberCol < 0)
          numberCol = 0;

        lcd.setCursor(13 + (numberCol > 1 ? numberCol + 1 : numberCol), 0);
        lcd.blink();

        break;
      }

    case 8:
      { // PIN Ekranı
        if (numberCol > 3 || numberCol < 0)
          numberCol = 0;

        lcd.setCursor(5 + numberCol, 1);
        lcd.blink();

        break;
      }
  }
}

void LcdTemplate::deleteNumber(LiquidCrystal &lcd) {
  switch (this->Type) {
    case 2:
      { // Manuel Ekran
        lcd.write('0');
        this->Out[numberCol] = '0';

        numberCol--;
        this->updateCursor(lcd);

        break;
      }
    case 4:
      { // Settings Ekran
        lcd.write('0');
        this->Out[numberCol] = '0';

        numberCol--;
        this->updateCursor(lcd);

        break;
      }

    case 8:
      { // PIN Ekranı
        lcd.write(' ');
        this->Out[numberCol] = ' ';

        numberCol--;
        this->updateCursor(lcd);

        break;

      }
  }
}

void LcdTemplate::HookKey(char key, LiquidCrystal &lcd) {
  if (this->IsVisible) {
    lcd.noBlink();
    lcd.noCursor();

    switch (this->Type) {
      case 1:
        { // ANA EKRAN
          if (key == 'A') {
            this->Type = 3; // Otomatik Ekrana geç
            this->Deter();

            this->Print(lcd);
          }
          else if (key == 'B') {
            this->Type = 2; // Manuel Ekrana geç
            this->Deter();

            this->Print(lcd);
            lcd.setCursor(11, 1);
            lcd.blink();
          }
          else if (key == 'C') {
            this->Type =  8; // Pin Ekranına git
            this->Deter();

            this->Print(lcd);
            lcd.setCursor(5, 1);
            lcd.blink();
          }

          break;
        }
      case 2:
        { // MANUEL EKRAN
          if (key == '*') { // geri
            this->Type = 1;
            this->Deter();

            this->Print(lcd);
          }
          else {
            this->HasFiredMotor = false;
            this->updateCursor(lcd);

            if (key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '#') {
              this->printNumber(key, lcd);
            }

            else if (key == 'C') {
              this->deleteNumber(lcd);
            }

            else if (key == 'D') {
              this->Type = 6;
              this->Deter();

              this->Print(lcd);
            }
          }

          break;
        }

      case 3:
        { // OTOMATIK EKRAN
          if (key == '*') { // geri
            this->Type = 1;
            this->Deter();

            this->Print(lcd);
          }

          break;
        }

      case 4:
        { // AYARLAR EKRANI
          if (key == '*') { // geri
            this->Type = 1;
            this->Deter();

            this->Print(lcd);
          }

          else {
            this->HasFiredMotor = false;
            this->updateCursor(lcd);

            if (key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '#') {
              this->printNumber(key, lcd);
            }

            else if (key == 'C') { // sil
              this->deleteNumber(lcd);
            }

            else if (key == 'D') { // kaydet
              uint8_t first = this->Out[0] - 48;
              uint8_t second = this->Out[1] - 48;
              uint8_t third = this->Out[2] - 48;
              uint8_t fourth = this->Out[3] - 48;

              EEPROM.write(0, first);
              EEPROM.write(1, second);
              EEPROM.write(2, third);
              EEPROM.write(3, fourth);

              this->Type = 7; // ana ekrana dön
              this->Deter();

              this->Print(lcd);
            }
          }

          break;
        }

      case 6:
        {

          break;
        }

      case 7:
        { //sensör kalibre
          break;
        }

      case 5:
        { // Pozisyona git Kalibrasyon

          break;
        }

      case 8:
        {
          if (key == '*') {
            this->Type = 1;
            this->Deter();

            this->Print(lcd);
          }
          else {
            this->updateCursor(lcd);

            if (key != 'A' && key != 'B' && key != 'C' && key != 'D' && key != '#') {
              this->printNumber(key, lcd);
            }

            else if (key == 'C') { // sil
              this->deleteNumber(lcd);
            }

            else if (key == 'D') { // onayla
              if (this->Out[0] == '0' && this->Out[1] == '0' && this->Out[2] == '0' && this->Out[3] == '0')
              {
                this->Type = 4; // ayarlar ekranına git
                this->Deter();

                this->Print(lcd);
                lcd.setCursor(13, 0);
                lcd.blink();
              }
              else {
                this->Type = 8;
                this->Deter();

                this->Print(lcd);
              }
            }
          }

          break;
        }


      default:
        {

          break;
        }
    }
  }
}



