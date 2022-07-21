#include <ArduinoJson.h> //v5.13.5
#include <HTTPClient.h>
#include <WiFiMulti.h>         // Thư viện dùng để kết nối WiFi đa cấu hình của ESP8266
#include <Arduino.h>
#include <HTTPUpdate.h>
#include "time.h"
//#include <WiFiManager.h>
WiFiMulti wifiMulti;           // Khởi tạo đối tượng wifiMulti từ ESP8266WiFiMulti
void update_FOTA();
uint32_t updateCounter = 0;
String version = "2.3";
String key = "37f0d021-9105-4822-8304-fed5b365c89d";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;
String gio, phut, giay;
int i;
String text;
int timer = 0;
void setup()
{
  Serial.begin(115200);
  Serial.println("Connecting ...");
  wifiMulti.addAP("baove", "");
  wifiMulti.addAP("TOTOLINK N300RH", "");
  wifiMulti.addAP("GreenFeed", "2020@GREENFEED");
  // Chờ kết nối WiFi được thiết lập. Quét tìm mạng WiFi và kết nối với mạng mạnh nhất.
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.println(i);
    i++;
    if (i >= 50) {
      esp_restart();
    }
  }
  Serial.println("\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());        // Cho biết tên mạng WiFi đã kết nối thành công
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());     // Gởi địa chỉ IP đến máy tinh
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}
char t[2];
void loop()
{
  HTTPClient http;
  http.begin("http://192.168.32.98:58888/api/refresh_service"); //My docker
  http.addHeader("Content-Type", "application/json");
  if (Serial.available() > 0) {
    //      int val = Serial.read();
    text = Serial.readStringUntil('\n');
    if (text == "60") {
      timer = 60;
      int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"True\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
      delay(20000);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      text = "";
    }
    if (text == "90") {
      timer = 90;
      int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"True\",\r\n     \"timer\":\"90\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
      delay(20000);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }
      http.end();
      text = "";
    }

  }
  if (text == "0") {
    int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"0\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
    delay(20000);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    text = "";
  }
  if (text == "checkout") {
    int httpResponseCode = http.POST("{\r\n  \"phonguv1\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"0\",\r\n     \"RECEIVE\":\"True\",\r\n     \"HUMAN_DETECT\":\"False\"},\r\n  \"phonguv2\":\r\n    {\"status\": \"False\",\r\n     \"timer\":\"60\",\r\n     \"RECEIVE\":\"False\",\r\n     \"HUMAN_DETECT\":\"False\"}\r\n}");
    delay(10000);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    text = "";
  }
  else {
    Serial.println("Error in WiFi connection");
  }
  if (updateCounter == 100)
    //  if (gio ==  "19" )
  {
    updateCounter = 0;
    Serial.println("Check update");
    update_FOTA();
  }
  /////////////////end boot////////////
  Serial.print("Ver: ");
  Serial.print(version);
  Serial.print(" ");
  Serial.print(gio); Serial.print("/"); Serial.print(phut); Serial.print("/"); Serial.println(giay);
  Serial.println(t);
  printLocalTime();
  delay(1000);
  updateCounter++;
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.println(i);
    i++;
    if (i >= 50) {
      esp_restart();
    }
  }
}
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
void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  //  Serial.println(&timeinfo,"%A, %d/%B/%Y %H:%M:%S");
  gio = timeinfo.tm_hour;
  phut = timeinfo.tm_min;
  giay = timeinfo.tm_sec;
  //  int result = val.toInt ();
}
