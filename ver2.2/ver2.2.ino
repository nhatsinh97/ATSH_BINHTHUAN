//#include <Watchdog.h>
/* Chương trình điều khiển phòng sát trùng sử dụng Arduino Nano */
#include <EEPROM.h>
#include <avr/wdt.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BigNumbers_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); //3F
BigNumbers_I2C bigNum(&lcd); // construct BigNumbers_I2C object, passing to it the name of our LCD object
String content = "";
int big1 = 0,big2 = 5, value, gio1, phut1, giay1;
char data;
byte x1 = 0, y1 = 1, x2 = 10, y2 = 1;
void setup()
{
  
  Serial.begin(115200);
  while (!Serial) {
    ResetBoard();
  }
  Wire.begin();
  bigNum.begin(); // set up BigNumbers
  lcd.init();
  lcd.backlight();
  lcd.print("  PHONG UV TU DONG"); lcd.setCursor(0 , 2); lcd.print("  Ver: 2.0"); delay(1000); lcd.clear();
  /* Start khai báo các chân I/O input và output */
  /* End khai báo các chân I/O input và output */
  wdt_enable(WDTO_8S);
}
void loop()
{
  big1++; big2++;
  
  if (big1 == 90){
    big1 = 0;
    big2 = 0;
  }
  wdt_reset ();
  while (!Serial) {
    ResetBoard();
  }
  lcdout();
  
  //----------------------------------------
//  big1 = value / 60;
//  big2 = value / 60;
  gio1 = value / 3600;
  phut1 = value % 3600 / 60;
  giay1 = value % 3600 % 60;
  //----------------------------------------
}
/* Start Chương trình làm chậm khi có sự kiện mở cữa lấy đồ  */
/* End Chương trình làm chậm khi có sự kiện mở cữa lấy đồ  */
void lcdout(){
  bigNum.displayLargeInt(big1, x1, y1, 2, false);
  lcd.setCursor(6 , 2); lcd.print("."); lcd.print(giay1); lcd.print(" ");
  bigNum.displayLargeInt(big2, x2, y2, 2, false);
  lcd.setCursor(16 , 2); lcd.print("."); lcd.print(giay1); lcd.print(" ");
  delay(1000);
}
void ResetBoard()
{
  asm volatile ( "jmp 0");
}
