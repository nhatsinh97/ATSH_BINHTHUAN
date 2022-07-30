#include <EEPROM.h>
String a = "";
void setup()
{
  // xóa các giá trị tại các ô nhớ EEPROM từ 0-511 (có 512 ô nhớ)
  for (int i = 0; i < 512; i++) {
    EEPROM.writeString(i, a);
    EEPROM.commit();
  }
   
  // Bật đèn led khi xóa xong 
  pinMode(17,OUTPUT);
  digitalWrite(17, LOW);
  digitalWrite(17, HIGH); 
}

void loop()
{
}
