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
String version = "2.11";
String key = "37f0d021-9105-4822-8304-fed5b365c89d";
char* server = "http://192.168.32.98:58888/api/refresh_service";
unsigned long previousMillis = 0;
const long interval = 1000; // giá trị delay (milliseconds)
int dem, i;
String sendRaspberry = "";
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
  pinMode(0, INPUT);
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
  http.begin(server);
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
//    if (status_uv1 > 0) {
//      if (status_uv1 == 1) {
//        datajson(String(backup), String(status_uv1), String(status_uv2), String(gio1), String(phut1), String(giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
//        delay(1000);
//      }
//      else {
//        datajson(String(backup), String(status_uv1), String(status_uv2), String(gio1), String(phut1), String(giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
//        
//      }
//    }
  }
  else {
    Serial2.print("Error on sending POST: ");
    Serial2.println(httpResponseCode);
  }
  http.end(); lcd.clear();
  update_FOTA();
}
//void checkButton() {
//  if ( digitalRead(0) == LOW ) {
//    delay(50);
//    if ( digitalRead(0) == LOW ) {
//      Serial.println("Button Pressed");
//      delay(3000); // reset delay hold
//      if ( digitalRead(0) == LOW ) {
//        Serial.println("Button Held");
//        Serial.println("Erasing Config, restarting");
//        wm.resetSettings();
//        ESP.restart();
//      }
//      Serial.println("Starting config portal");
//      wm.setConfigPortalTimeout(120);
//      if (!wm.startConfigPortal("ATSH-AP", "")) {
//        Serial.println("failed to connect or hit timeout");
//        delay(3000);
//        // ESP.restart();
//      } else {
//        //if you get here you have connected to the WiFi
//        Serial.println("connected...yeey :)");
//        backup_ota();
//      }
//    }
//  }
//}
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
//  checkButton();
  unsigned long currentMillis = millis();
  //************************ CHẾ ĐỘ SETUP ****************************//
  //************************ NÚT NHẤN 1 ******************************//
  if ( digitalRead(0) == LOW ) {
    datajson(String(backup), String(status_uv1), String(status_uv2), String(gio1), String(phut1), String(giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
      
    
  }

  //**** START NÚT NHẤN MỞ CỮA PHÒNG UV1 ****//
  //**** END NÚT NHẤN PHÒNG UV1 ****//

  //**** START NÚT NHẤN MỞ CỮA PHÒNG UV2 ****//
  //**** END NÚT NHẤN PHÒNG UV2 ****//

  if (dem == 60) {
    dem = 0;
    datajson(String(backup), String(status_uv1), String(status_uv2), String(gio1), String(phut1), String(giay1), String(gio2), String(phut2), String(giay2), String(cb_cua1), String(cb_cua2));
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

//**** SEND DATA JSON TO RASPBERRY ****//
void datajson(String backup, String status_uv1, String status_uv2, String gio1, String phut1, String giay1, String gio2, String phut2, String giay2, String cb_cua1, String cb_cua2) {
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service");
  http.addHeader("Content-Type", "application/json");
  sendRaspberry = "";
  sendRaspberry = "{\"backup\": \"" + String(backup) + "\"," +
                  "\"restart\": \"False\"," +
                  "\"nharuaxe\": \"False\"," +
                  "\"phonguv1\": { \"status_uv1\":\"" + String(status_uv1) + "\"," +
                  "\"cb_cua1\": \"" + String(cb_cua1) + "\"," +
                  "\"gio1\":  \"" + String(gio1)  + "\"," +
                  "\"phut1\": \"" + String(phut1) + "\"," +
                  "\"giay1\": \"" + String(giay1) + "\"}" +
                  "," +
                  "\"phonguv2\": { \"status_uv2\":\"" + String(status_uv2) + "\"," +
                  "\"cb_cua2\": \"" + String(cb_cua2) + "\"," +
                  "\"gio2\":  \"" + String(gio2)  + "\"," +
                  "\"phut2\": \"" + String(phut2) + "\"," +
                  "\"giay2\": \"" + String(giay2) + "\"}" + "}";

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
