/* Chương trình điều khiển phòng sát trùng sử dụng Arduino Nano */
#include <EEPROM.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BigNumbers_I2C.h>
<<<<<<< HEAD
LiquidCrystal_I2C lcd(0x3F, 20, 4); //27
=======
LiquidCrystal_I2C lcd(0x3F, 20, 4); //3F
>>>>>>> d64c1c7c4b697e12a7c655852ac28a7b05cfb7fd
BigNumbers_I2C bigNum(&lcd); // construct BigNumbers_I2C object, passing to it the name of our LCD object
/* Start khai báo các biến khi khởi động */
int RECEIVE = A3 , led = 13, checkout = A0, trai = A1, bep = A2, uv = 11, ozon = 12;
long value = 0 ; int dem = 0, gio = 0, phut = 0, giay = 0, big = 0, check = 0, i = 0, reset = 0;
char b[100]       = "NHA BEP";
char e[100]       = "Backup ";
char a[100]       = "NV TRAI";
char d[100]       = " OFF   ";
char f[100]       = " ON    ";
char startus[100] = " OFF   ";
char gui[10]      = "1";
String content = "";
char data;
/* End khai báo các biến khi khởi động */
byte x = 7;//x & y determines position of character on screen
byte y = 0;
void setup()
{
  Serial.begin(115200);
//  while (!Serial) {
//    ResetBoard();
//  }
  Wire.begin();
  bigNum.begin(); // set up BigNumbers
  lcd.init();
  lcd.backlight();
  lcd.print("  PHONG UV TU DONG"); lcd.setCursor(0 , 2); lcd.print("  Ver: 2.0"); delay(1000); lcd.clear();
  strcpy(startus, e);
  /* Start khai báo các chân I/O input và output */
  pinMode(trai, INPUT_PULLUP); pinMode(bep, INPUT_PULLUP);
  digitalWrite(trai, HIGH)   ; digitalWrite(bep, HIGH)   ;
  pinMode(checkout, INPUT_PULLUP); pinMode(RECEIVE, INPUT_PULLUP);
  pinMode(uv, OUTPUT); digitalWrite(uv, LOW);
  pinMode(ozon, OUTPUT); digitalWrite(ozon, LOW);
  pinMode(led, OUTPUT); digitalWrite(led, LOW);
  /* End khai báo các chân I/O input và output */
  wdt_enable(WDTO_8S);
}
void loop()
{
  while (!Serial) {
    ResetBoard();
  }
  wdt_reset ();
  if ((value == 0) && (digitalRead (trai) == 0)) {
    strcpy(gui, "60");
    strcpy(startus, a); lcd.setCursor(9 , 2); Serial.println(gui);
    lcd.print("          "); lcd.setCursor(9 , 2);
    lcd.print(startus);
    value = 3600;
  }
  if ((value == 0) && (digitalRead (bep) == 0)) {
    strcpy(gui, "60");
    strcpy(startus, b); lcd.setCursor(9 , 2); Serial.println(gui);
    lcd.print("          "); lcd.setCursor(9 , 2);
    lcd.print(startus);
    value = 3600;
  }
  if ((digitalRead (trai) == 0) && (digitalRead(bep) == 0)) {
    for ( i = 0; i <= 30; i++) {
      lcd.setCursor(9 , 2);
      lcd.print("          "); lcd.setCursor(8 , 2);
      lcd.print("Goto Reset"); lcd.print(i); wdt_reset ();
      if ((digitalRead (trai) == 0) && (digitalRead(bep) == 0)) {
        reset++;
        delay(250);
      }
      else  {
        if ((reset > 1) && (reset < 5)) {
          lcd.backlight();
        }
        if ((reset > 5) && (reset < 10)) {
          lcd.noBacklight();
        }
        if ((reset > 10) && (reset < 25)) {
          strcpy(startus, "Reset ");
          lcd.setCursor(0 , 1); lcd.print("Timer :");
          bigNum.displayLargeInt(big, x, y, 2, false);
          lcd.setCursor(13 , 1); lcd.print("."); lcd.print(giay); lcd.print(" ");
          lcd.setCursor(0 , 2); lcd.print("Status:");
          lcd.setCursor(8 , 2); lcd.print("            ");
          lcd.setCursor(9 , 2); lcd.print(startus);
          lcd.setCursor(0 , 3); lcd.print("Vi Tri:");
          lcd.setCursor(7 , 3); lcd.print("Cong Bao Ve");
          value = 0;
          wdt_reset (); delay(4000); //3600
        }
        if ((reset > 25) && (reset < 29)) {
          strcpy(startus, "test "); lcd.setCursor(9 , 2); lcd.print(startus);
          test();
        }
        reset = 0;
        lcd.setCursor(8 , 2);
        lcd.print("            ");
        delay(500);
        break;
      }
      if (reset == 30) {
        lcd.setCursor(8 , 2);
        lcd.print("            ");
        strcpy(startus, "Restart "); lcd.setCursor(9 , 2);
        lcd.print(startus);
        value = 0;
        ResetBoard();
      }
    }
    wdt_reset ();

  }
  if (digitalRead(checkout) == 0) {
    delay(250);
    if (digitalRead(checkout) == 0) {
      lcd.setCursor(9 , 2);
      lcd.print("        ");
      strcpy(startus, "checkout"); lcd.setCursor(9 , 2);
      lcd.print(startus);
      Serial.println("checkout"); delay(50);
      delaycheck();
      lcd.setCursor(9 , 2);
      lcd.print("            ");
      strcpy(startus, f);
    }
  }
  if (digitalRead(RECEIVE) == 0) {
    delay(250);
    if (digitalRead(RECEIVE) == 0) {
      lcd.setCursor(9 , 2);
      lcd.print("        ");
      strcpy(startus, "RECEIVE"); lcd.setCursor(9 , 2);
      lcd.print(startus);
      Serial.println("RECEIVE"); delay(50);
      delayRECEIVE();
      lcd.setCursor(9 , 2);
      lcd.print("            ");
      strcpy(startus, f);
    }
  }
  //----------------------------------------
  big = value / 60;
  gio = value / 3600;
  phut = value % 3600 / 60;
  giay = value % 3600 % 60;
  //----------------------------------------
  if (value > 0) {
    lcd.setCursor(0 , 1); lcd.print("Timer :");
    bigNum.displayLargeInt(big, x, y, 2, false);
    lcd.setCursor(13 , 1); lcd.print("."); lcd.print(giay); lcd.print(" ");
    digitalWrite(led, HIGH); digitalWrite(uv, HIGH);
    lcd.setCursor(0 , 2); lcd.print("Status:");
    lcd.setCursor(9 , 2); lcd.print(startus);
    lcd.setCursor(0 , 3); lcd.print("Vi Tri:");
    lcd.setCursor(7 , 3); lcd.print("Cong Bao Ve");
    delay(980); //980
    value--;
    if (gio == 1) {
      digitalWrite(ozon, HIGH);
    }
    if (phut > 30) {
      digitalWrite(ozon, HIGH);
    }
    if ((gio == 0) && (phut < 30)) {
      digitalWrite(ozon, LOW);
    }
  }
  if (value == 0) {
    digitalWrite(led, LOW);
    digitalWrite(uv, LOW); strcpy(startus, d); strcpy(gui, "0");
    lcd.setCursor(0 , 1); lcd.print("Timer :");
    bigNum.displayLargeInt(big, x, y, 2, false);
    lcd.setCursor(13 , 1); lcd.print("."); lcd.print(giay); lcd.print(" ");
    lcd.setCursor(0 , 2); lcd.print("Status:");
    lcd.setCursor(9 , 2); lcd.print(startus);
    lcd.setCursor(0 , 3); lcd.print("Vi Tri:");
    lcd.setCursor(7 , 3); lcd.print("Cong Bao Ve");
  }
  dem++;
  //  Serial.println(dem);
  if (dem == 50) {
    wdt_reset ();
    dem = 0;
    Serial.println(gui);

  }
}
/* Start Chương trình làm chậm khi có sự kiện mở cữa lấy đồ  */
void delaycheck() {              /*  sự kiện mở cữa phòng uv  */
  for (check = 1; check <= 600 ; check++) {
    wdt_reset ();
    lcd.setCursor(9 , 2);
    lcd.print("          ");
    lcd.setCursor(9 , 2);
    //    lcd.print("Gui data");
    lcd.print("Nhan do ");
    if (digitalRead(checkout) == 0) {
      delay(980); //980
      value--;
    }
    if (digitalRead(checkout) == 1) {
      break;
    }
  }
}
void delayRECEIVE() {              /*  sự kiện nhận diện chuyển động trong phòng uv  */
  for (check = 1; check <= 600 ; check++) {
    wdt_reset ();
    lcd.setCursor(9 , 2);
    lcd.print("          ");
    lcd.setCursor(9 , 2);
    //    lcd.print("Gui data");
    lcd.print("Gui vd vao");
    if (digitalRead(RECEIVE) == 0) {
      delay(980); //980
      value--;
    }
    if (digitalRead(RECEIVE) == 1) {
      break;
    }
  }
}
void test() {
  for (check = 1; check <= 300 ; check++) {
    wdt_reset ();
    value = check;
    big = value / 60;
    gio = value / 3600;
    phut = value % 3600 / 60;
    giay = value % 3600 % 60;
    bigNum.displayLargeInt(big, x, y, 2, false);
    lcd.setCursor(13 , 1); lcd.print("."); lcd.print(giay); lcd.print(" ");
    lcd.setCursor(8 , 2);
    lcd.print("            ");
    lcd.setCursor(9 , 2);
    lcd.print("test");
    if ((check > 1) && (check < 60)) {
      digitalWrite(led, HIGH); digitalWrite(ozon, HIGH); digitalWrite(uv, HIGH);
    }
    if ((check > 60) && (check < 299)) {
      digitalWrite(led, HIGH); digitalWrite(ozon, LOW);
    }
    if (check == 300) {
      digitalWrite(led, LOW); digitalWrite(uv, LOW);
      lcd.setCursor(9 , 2);
      lcd.print("          ");
      lcd.setCursor(9 , 2);
      lcd.print(startus);
      value = 0;
    }
    delay(1000);
  }
}
/* End Chương trình làm chậm khi có sự kiện mở cữa lấy đồ  */
void ResetBoard()
{
  asm volatile ( "jmp 0");
}
