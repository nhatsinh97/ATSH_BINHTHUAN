/* Chương trình điều khiển phòng sát trùng sử dụng Arduino Nano */
#include <EEPROM.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
/* Start khai báo các biến khi khởi động */
int RECEIVE = A3 , led = 13, checkout = A0, trai = A1, bep = A2, uv = 11, ozon = 12, demmenu = 0;
unsigned int address = 0; long value ; int dem = 0, gio = 0, phut = 0, giay = 0, check = 0, i = 0, reset = 0;
int mnnhabep; int mnnhanvien;
char b[100]       = "NHA BEP   ";
char e[100]       = "BACKUP    ";
char a[100]       = "NV TRAI   ";
char d[100]       = " OFF      ";
char f[100]       = " ON       ";
char startus[100] = " OFF      ";
char gui[10]      = "1";
String content = "";
char data;
/* End khai báo các biến khi khởi động */
void setup()
{
  Serial.begin(115200);
  while (!Serial) {
  }
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.print("  PHONG UV TU DONG"); lcd.setCursor(0 , 2); lcd.print("  Ver: 1.0"); delay(2000); lcd.clear();
  lcd.print("   SMART UV OZONE"); lcd.setCursor(0 , 1); lcd.print("  Farm Lang viet 2");
  lcd.setCursor(0 , 2); lcd.print("Status :"); lcd.setCursor(9 , 2); strcpy(startus, e); lcd.print(startus);
  lcd.setCursor(0 , 3); lcd.print("Timer  :"); lcd.setCursor(9 , 3);
  lcd.print(gio); lcd.print(":"); lcd.print(phut); lcd.print(":"); lcd.print(giay); lcd.print("  ");
  /* Start khai báo các chân I/O input và output */
  pinMode(trai, INPUT_PULLUP); pinMode(bep, INPUT_PULLUP);
  digitalWrite(trai, HIGH)   ; digitalWrite(bep, HIGH)   ;
  pinMode(checkout, INPUT_PULLUP); pinMode(RECEIVE, INPUT_PULLUP);
  pinMode(uv, OUTPUT); digitalWrite(uv, LOW);
  pinMode(ozon, OUTPUT); digitalWrite(ozon, LOW);
  pinMode(led, OUTPUT); digitalWrite(led, LOW);
  /* End khai báo các chân I/O input và output */
  wdt_enable(WDTO_8S);
  value = EEPROMReadlong(address);
}
void loop()
{
  wdt_reset ();
  if ((value == 0) && (digitalRead (trai) == 0)) {
    gui [10] = "90";
    strcpy(startus, a); lcd.setCursor(9 , 2);
    lcd.print("          "); lcd.setCursor(9 , 2);
    lcd.print(startus);
    EEPROMWritelong(address, 5400); delay(5); //3600
  }
  if ((value == 0) && (digitalRead (bep) == 0)) {
    gui [10] = "90";
    strcpy(startus, b); lcd.setCursor(9 , 2);
    lcd.print("          "); lcd.setCursor(9 , 2);
    lcd.print(startus);
    EEPROMWritelong(address, 5400); delay(5);
  }
  if ((digitalRead (trai) == 0) && (digitalRead(bep) == 0)) {
    for ( i = 0; i <= 30; i++) {
      lcd.setCursor(9 , 2);
      lcd.print("          "); lcd.setCursor(8 , 2);
      lcd.print("Goto Reset"); lcd.print(i); wdt_reset ();
      if ((digitalRead (trai) == 0) && (digitalRead(bep) == 0)) {
        reset++;
        delay(500);
      }
      else  {
        if ((reset > 1) && (reset < 5)) {
          lcd.backlight();
        }
        if ((reset > 5) && (reset < 10)) {
          lcd.noBacklight();
        }
        if ((reset > 10) && (reset < 25)) { lcd.setCursor(8 , 2); lcd.print("            ");
          strcpy(startus, "Reset "); lcd.setCursor(9 , 2); lcd.print(startus); lcd.setCursor(9 , 3); lcd.print("0");
          lcd.print(":"); lcd.print("0"); lcd.print(":"); lcd.print("0"); lcd.print("    ");
          EEPROMWritelong(address, 0); wdt_reset (); delay(4000); //3600
        }
        if ((reset > 25) && (reset < 29)) {
          strcpy(startus, "test "); lcd.setCursor(9 , 2); lcd.print(startus); lcd.setCursor(9 , 3); lcd.print("0");
          lcd.print(":"); lcd.print("0"); lcd.print(":"); lcd.print("0"); lcd.print("    ");
          test();
        }
        reset = 0;
        lcd.setCursor(8 , 2);
        lcd.print("            ");
        delay(1000);
        break;
      }
      if (reset == 30) {
        lcd.setCursor(8 , 2);
        lcd.print("            ");
        strcpy(startus, "Restart "); lcd.setCursor(9 , 2);
        lcd.print(startus);
        EEPROMWritelong(address, 0);
        delay(10000);
      }
    }
    wdt_reset ();

  }
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
  //----------------------------------------
  value = EEPROMReadlong(address);
  gio = value / 3600;
  phut = value % 3600 / 60;
  giay = value % 3600 % 60;
  //----------------------------------------
  if (value > 0) {
    digitalWrite(led, HIGH); strcpy(gui, "90");
    digitalWrite(uv, HIGH);
    lcd.setCursor(9 , 2); lcd.print(startus);
    lcd.setCursor(9 , 3); lcd.print(gio);
    lcd.print(":"); lcd.print(phut); lcd.print(":"); lcd.print(giay); lcd.print(" ");
    delay(980);
    value--; EEPROMWritelong(address, value); delay(5);
    //----------------------------------------
    if ((gio == 0) && (phut < 30)) {
      digitalWrite(ozon, LOW);
    }
  }
  else {
    digitalWrite(led, LOW);
    digitalWrite(uv, LOW); strcpy(startus, d); strcpy(gui, "0");
    lcd.setCursor(9 , 2); lcd.print(startus); lcd.print(" ");
    lcd.setCursor(9 , 3); lcd.print(gio);
    lcd.print(":"); lcd.print(phut); lcd.print(":"); lcd.print(giay);
  }
  dem++;
  if (dem == 40) {
    wdt_reset ();
    dem = 0;
    Serial.println(gui);

  }
}
//}
/* Start Chương trình làm chậm khi có sự kiện mở cữa lấy đồ  */
void delaycheck() {
  for (check = 1; check <= 600 ; check++) {
    wdt_reset ();
    lcd.setCursor(9 , 2);
    lcd.print("          ");
    lcd.setCursor(9 , 2);
    lcd.print("Gui data");
    if (digitalRead(checkout) == 0) {
      delay(500);
    }
    if (digitalRead(checkout) == 1) {
      break;
    }
  }
}
void delayRECEIVE() {
  for (check = 1; check <= 600 ; check++) {
    wdt_reset ();
    lcd.setCursor(9 , 2);
    lcd.print("          ");
    lcd.setCursor(9 , 2);
    lcd.print("Gui data");
    if (digitalRead(RECEIVE) == 0) {
      delay(500);
    }
    if (digitalRead(RECEIVE) == 1) {
      break;
    }
  }
}
void test() {
  for (check = 1; check <= 300 ; check++) {
    wdt_reset ();
    lcd.setCursor(8 , 2);
    lcd.print("            ");
    lcd.setCursor(9 , 2);
    lcd.print("test("); lcd.print(check); lcd.print(")");
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
      EEPROMWritelong(address, 0);
    }
    delay(1000);
  }
}
/* End Chương trình làm chậm khi có sự kiện mở cữa lấy đồ  */

/*EEPROM WRITE/READ*/
//This function will write a 4 byte (32bit) long to the eeprom at
//the specified address to adress + 3.
void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}
//This function will return a 4 byte (32bit) long from the eeprom
//at the specified address to adress + 3.
long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
/*End EEPROM WRITE/READ*/
