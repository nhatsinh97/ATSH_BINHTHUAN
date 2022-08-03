#include <WiFiManager.h>
#include <ArduinoJson.h>
#include "EEPROM.h"
#include <Wire.h>
#include <Modbus.h>
#include <ModbusMaster.h>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include "time.h"
#include <Adafruit_MCP4725.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); //3F
void update_FOTA();
String backup = "False";
String version = "2.12";
String key = "37f0d021-9105-4822-8304-fed5b365c89d";
unsigned long previousMillis = 0;
const long interval = 1000; // giá trị delay (milliseconds)
int dem, i;
char* sendRaspberry = "";
int status_uv1, status_uv2;
int gio1, phut1, giay1;
int gio2, phut2, giay2;
long cb_cua1 = 0, cb_cua2 = 0;
//int RL_1 = 15, RL_2 = 13, RL_3 = 12, RL_4 = 32, RL_5 = 33, RL_6 = 25;
int BT_1 = 36, BT_2 = 39, BT_cua1 = 34, BT_cua2 = 4;
#define SerialComputer  Serial
#define SerialModbus    Serial2
Adafruit_MCP4725 dac1;
Adafruit_MCP4725 dac2;
bool wm_nonblocking = false;
WiFiManager wm;
WiFiManagerParameter custom_field;
void setup() {
  pinMode(BT_1, INPUT_PULLUP);    pinMode(BT_2, INPUT_PULLUP);
  pinMode(BT_cua1, INPUT_PULLUP); pinMode(BT_cua2, INPUT_PULLUP);
//  pinMode(RL_1, OUTPUT); pinMode(RL_2, OUTPUT);
//  pinMode(RL_3, OUTPUT); pinMode(RL_4, OUTPUT);
//  pinMode(RL_5, OUTPUT); pinMode(RL_6, OUTPUT);
//  digitalWrite(RL_1, LOW); digitalWrite(RL_2, LOW);
//  digitalWrite(RL_3, LOW); digitalWrite(RL_4, LOW);
//  digitalWrite(RL_5, LOW); digitalWrite(RL_6, LOW);
  WiFi.mode(WIFI_STA);
  SerialComputer.begin(115200); SerialModbus.begin(115200);
  dac1.begin(0x60); dac2.begin(0x61);
  dac1.setVoltage(0, false); dac2.setVoltage(0, false);
  Wire.begin(); lcd.init(); lcd.backlight();
  lcd.print("  PHONG UV TU DONG"); lcd.setCursor(0 , 2); lcd.print(version); //delay(1000); lcd.clear();
  Serial2.setDebugOutput(true);
  delay(3000); lcd.clear(); lcd.print("  DANG NAP DU LIEU"); lcd.setCursor(0 , 2); lcd.print("  Update Firmware");
  if (wm_nonblocking) wm.setConfigPortalBlocking(false);
  int customFieldLength = 40;
  const char* custom_radio_str = "<br/><label for='customfieldid'>Custom Field Label</label><input type='radio' name='customfieldid' value='1' checked> One<br><input type='radio' name='customfieldid' value='2'> Two<br><input type='radio' name='customfieldid' value='3'> Three";
  new (&custom_field) WiFiManagerParameter(custom_radio_str);
  wm.addParameter(&custom_field);
  wm.setSaveParamsCallback(saveParamCallback);
  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
  wm.setMenu(menu);
  wm.setClass("invert");
  wm.setConfigPortalTimeout(30);
  bool res;
  res = wm.autoConnect("AutoConnect-ATSH", "");
  if (!res) {
    Serial2.println("Failed to connect or hit timeout");
    // ESP.restart();
  }
  else {
    Serial2.println("connected...yeey :)");
    backup_ota();
  }

}
void backup_ota() {
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST("{\"backup\":\"True\"}");
  delay(1000);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial2.println(httpResponseCode);
    Serial2.println(response);
    StaticJsonDocument<500> doc;
    deserializeJson(doc, response);
    backup = "False";
    gio1  = doc["phonguv1"]["gio1"];
    phut1 = doc["phonguv1"]["phut1"];
    giay1 = doc["phonguv1"]["giay1"];
    status_uv1 = doc["phonguv1"]["status_uv1"];
    gio2  = doc["phonguv2"]["gio2"];
    phut2 = doc["phonguv2"]["phut2"];
    giay2 = doc["phonguv2"]["giay2"];
    status_uv2 = doc["phonguv2"]["status_uv2"];
    if (status_uv1 > 0) {
      if (status_uv1 == 1) {
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
        delay(1000);
      }
      else {
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
        
      }
    }
    if (status_uv2 > 0) {
      if (status_uv2 == 1) {
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
        delay(1000);
      }
      else {
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
        
      }
    }
  }
  else {
    Serial2.print("Error on sending POST: ");
    Serial2.println(httpResponseCode);
  }
  http.end(); lcd.clear();
  update_FOTA();
}
void checkButton() {
  if ( digitalRead(0) == LOW ) {
    delay(50);
    if ( digitalRead(0) == LOW ) {
      Serial2.println("Button Pressed");
      delay(3000); // reset delay hold
      if ( digitalRead(0) == LOW ) {
        Serial2.println("Button Held");
        Serial2.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }
      Serial2.println("Starting config portal");
      wm.setConfigPortalTimeout(120);
      if (!wm.startConfigPortal("ATSH-AP", "")) {
        Serial2.println("failed to connect or hit timeout");
        delay(3000);
        // ESP.restart();
      } else {
        //if you get here you have connected to the WiFi
        Serial2.println("connected...yeey :)");
        backup_ota();
      }
    }
  }
}
String getParam(String name) {
  String value;
  if (wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}
void saveParamCallback() {
  Serial2.println("[CALLBACK] saveParamCallback fired");
  Serial2.println("PARAM customfieldid = " + getParam("customfieldid"));
}
void loop() {
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service");
  http.addHeader("Content-Type", "application/json");
  if (wm_nonblocking) wm.process(); // avoid delays() in loop when non-blocking and other long running code
  checkButton();
  unsigned long currentMillis = millis();
  //************************ CHẾ ĐỘ SETUP ****************************//
  //************************ NÚT NHẤN 1 ******************************//
  if ((status_uv1 == 0) && (digitalRead (BT_1) == 0) && (digitalRead (BT_2) == 1)) {
    delay(150);
  }
  if ((status_uv1 == 0) && (digitalRead (BT_1) == 0) && (digitalRead (BT_2) == 1)) {
    delay(150);
  }
  if ((status_uv1 == 0) && (digitalRead (BT_1) == 0) && (digitalRead (BT_2) == 1)) {
    delay(150);
  }

  //**** START NÚT NHẤN MỞ CỮA PHÒNG UV1 ****//
  if (digitalRead (BT_cua1) == 0) {
    cb_cua1 = 1;
    for (i = 1; i <= 100; i++) {
      lcd.clear();
      lcd.setCursor(0 , 0); lcd.print("Chu y: Cua so 2");
      lcd.setCursor(0 , 1); lcd.print("       dang mo !");
      lcd.setCursor(9 , 2); lcd.print(i);
      if (i == 1) {
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
      }
      if (digitalRead (BT_cua1) == 1) {
        cb_cua1 = 0;
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
        lcd.clear();
        break;
      }
      delay(1000);
    }
  }
  //**** END NÚT NHẤN PHÒNG UV1 ****//

  //**** START NÚT NHẤN MỞ CỮA PHÒNG UV2 ****//
  if (digitalRead (BT_cua2) == 0) {
    for (i = 1; i <= 100; i++) {
      lcd.clear();
      lcd.setCursor(0 , 0); lcd.print("Chu y: Cua so 1");
      lcd.setCursor(0 , 1); lcd.print("       dang mo !");
      lcd.setCursor(9 , 2); lcd.print(i);
      cb_cua2 = 1;
      if (i == 1) {
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
      }
      if (digitalRead (BT_cua2) == 1) {
        cb_cua2 = 0;
        datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
        lcd.clear();
        break;
      }
      delay(1000);
    }
  }
  //**** END NÚT NHẤN PHÒNG UV2 ****//

  if (dem == 60) {
    dem = 0;
    datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
    //**** START Nếu phòng UV được bật thì lưu dữ liệu vào eeprom ****//
    int httpResponseCode = http.POST(sendRaspberry);
    delay(1000);
    http.end(); lcd.clear();
    //**** END Nếu phòng UV được bật thì lưu dữ liệu vào eeprom ****//
  }
  //***** lấy giá trị hiện tại trừ đi giá trị trước đó và so sánh với khoản thời gian delay ******//
  if (currentMillis - previousMillis >= interval) {
    // nếu giá trị đó bằng hoặc vượt qua
    // lưu giá trị hiện tại thành giá trị lần cuối cập nhật
    previousMillis = currentMillis; dem++;
  }
  //nếu giá trị chưa vượt qua thì không làm gì cả
}
//**** START PHÒNG UV 1 ****//
void off1() {
  gio1 = 0, phut1 = 0, giay1 = 0, status_uv1 = 0;
  datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
  ESP.restart();
}
//**** END PHÒNG UV1 ****//
//**** START PHÒNG UV2 ****//
void off2() {
  gio2 = 0, phut2 = 0, giay2 = 0, status_uv2 = 0;
  datajson(char*(backup), char*(status_uv1), char*(status_uv2), char*(gio1), char*(phut1), char*(giay1), char*(gio2), char*(phut2), char*(giay2), char*(cb_cua1), char*(cb_cua2));
  ESP.restart();
}
//**** END PHÒNG UV 2 ****//

//**** SEND DATA JSON TO RASPBERRY ****//
void datajson(char* backup,char* status_uv1,char* status_uv2,char* gio1,char* phut1,char* giay1,char* gio2,char* phut2,char* giay2,char* cb_cua1,char* cb_cua2) {
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service");
  http.addHeader("Content-Type", "application/json");
  sendRaspberry = "";
  sendRaspberry = "{\"backup\": \"" + char(backup) + "\"," +
                  "\"phonguv1\": { \"status_uv1\":\"" + char(status_uv1) + "\"," +
                  "\"cb_cua1\": \"" + char(cb_cua1) + "\"," +
                  "\"gio1\":  \"" + char(gio1)  + "\"," +
                  "\"phut1\": \"" + char(phut1) + "\"," +
                  "\"giay1\": \"" + char(giay1) + "\"}" +
                  "," +
                  "\"phonguv2\": { \"status_uv2\":\"" + char(status_uv2) + "\"," +
                  "\"cb_cua2\": \"" + char(cb_cua2) + "\"," +
                  "\"gio2\":  \"" + char(gio2)  + "\"," +
                  "\"phut2\": \"" + char(phut2) + "\"," +
                  "\"giay2\": \"" + char(giay2) + "\"}" + "}";

  StaticJsonDocument<500> doc;
  deserializeJson(doc, sendRaspberry);
  serializeJsonPretty(doc, Serial2);
  int httpResponseCode = http.POST(sendRaspberry);
  delay(1000);
  http.end();
}
//**** END SEND DATA JSON TO RASPBERRY ****//
String getChipId()
{
  String ChipIdHex = String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);
  ChipIdHex += String((uint32_t)ESP.getEfuseMac(), HEX);
  return ChipIdHex;
}

void update_FOTA()
{
  String url = "http://otadrive.com/deviceapi/update?";
  url += "k=" + key;
  url += "&v=" + version;
  url += "&s=" + getChipId(); // định danh thiết bị trên Cloud

  WiFiClient client;
  httpUpdate.update(client, url, version);
}
