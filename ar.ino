#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);
int nutthucpham = 7;
int nutvatdung = 8;
int checkout = A0;
int thucpham = A1;
int vatdung = A2;
int uv = 11, ozon = 12;
char a[100] = "ON 60";
char e[100] = "BACKUP";
char b[100] = "ON 90";
char d[100] = "OFF";
char startus[100] = "OFF";
unsigned int address = 0;
//byte value;
long value;
int dem = 0;
int gio = 0;
int phut = 0;
int giay = 0;
char gui[10] = "1";
void setup()
{
  Wire.begin();
  lcd.init();
  Serial.begin(115200);
  lcd.backlight();
  lcd.setCursor(0 , 0);
  lcd.print("  NGUYEN NHAT SINH"); lcd.setCursor(0 , 2); lcd.print("  PHONG UV TU DONG"); delay(500);
  lcd.clear();lcd.setCursor(0 , 0);
  lcd.print("   SMART UV OZONE"); lcd.setCursor(0 , 1); lcd.print("   Farm Binh Thuan");
  lcd.setCursor(0 , 2); lcd.print("Status :"); lcd.setCursor(9 , 2); strcpy(startus, e);
  lcd.print(startus); lcd.setCursor(0 , 3); lcd.print("Time   :"); lcd.setCursor(9 , 3);
  lcd.print(gio); lcd.print(":"); lcd.print(phut); lcd.print(":"); lcd.print(giay);
  pinMode(thucpham, INPUT_PULLUP); pinMode(vatdung, INPUT_PULLUP);
  digitalWrite(thucpham, HIGH); digitalWrite(vatdung, HIGH);
  pinMode(checkout, INPUT_PULLUP);
  pinMode(uv, OUTPUT); digitalWrite(uv, LOW);
  pinMode(ozon, OUTPUT); digitalWrite(ozon, LOW);
  pinMode(nutthucpham, OUTPUT); digitalWrite(nutthucpham, LOW);
  pinMode(nutvatdung, OUTPUT); digitalWrite(nutvatdung, LOW);
}
void loop()
{
  // start chuyển giây thành giờ, phút, giây
  value = EEPROMReadlong(address);
  gio = value / 3600;
  phut = value % 3600 / 60;
  giay = value % 3600 % 60;
  // end chuyển giây thành giờ, phút, giây
  //-----------------------------------------------------
  if (digitalRead(thucpham) == 0) { // 60 phút
    digitalWrite(nutthucpham, HIGH); gui [10] = "60";
    strcpy(startus, a); lcd.setCursor(9 , 2);
    lcd.print(startus);
    EEPROMWritelong(address, 3600); delay(5);
  }
  if (digitalRead(vatdung) == 0) { // 90 phút
    digitalWrite(nutvatdung, HIGH); gui [10] = "90";
    strcpy(startus, b); lcd.setCursor(9 , 2);
    lcd.print(startus);
    EEPROMWritelong(address, 5400); delay(5);
  }
  
  if (digitalRead(checkout) == 0) {
    delay(100); Serial.println("checkout"); delay(100); }
  //------------------------------------------------------
  // start nếu data lớn hơn 0
  if (value > 0) {
    digitalWrite(uv, HIGH);
    lcd.setCursor(0 , 2); lcd.print("Status :"); lcd.setCursor(9 , 2); strcpy(startus, d); lcd.setCursor(0 , 3); 
    lcd.print("Time   :"); lcd.setCursor(9 , 3); lcd.print(gio); lcd.print(":"); lcd.print(phut); lcd.print(":"); 
    lcd.print(giay); lcd.print("   "); delay(970); value--; EEPROMWritelong(address, value); delay(5);
    // start nếu nhấn nút sát trùng gửi data tới raspberry
    if (gio == 1) { strcpy(gui, "90"); digitalWrite(ozon, HIGH);}
    else{ strcpy(gui, "60"); }
    // end gửi
    if (phut > 30) {
      digitalWrite(ozon, HIGH);
    }
    if ((gio == 0) && (phut < 30)) {
      digitalWrite(ozon, LOW);
    }
  }
  // end nếu data lớn hơn 0
  
  //----- start nếu data bằng 0
  else {
    digitalWrite(uv, LOW); strcpy(startus, d);
    lcd.setCursor(0 , 2); strcpy(gui, "0");
    lcd.print("Status :"); lcd.setCursor(9 , 2);
    lcd.print(startus); lcd.print("   ");
    lcd.setCursor(15 , 2); lcd.print(" ");
    lcd.setCursor(0 , 3); lcd.print("Time   :"); lcd.setCursor(9 , 3); lcd.print(gio); lcd.print(":"); lcd.print(phut); lcd.print(":"); lcd.print(giay);
    digitalWrite(nutthucpham, LOW);
    digitalWrite(nutvatdung, LOW);
  }
 //------ end nếu data bằng 0
  
//------ start tăng bộ đếm để gửi data tới raspberry nếu được 30 lần thì gửi dữ liệu
  dem++;
  if (dem == 30) { dem = 0; Serial.println(gui); }
}
//------ end đếm ----------------


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
