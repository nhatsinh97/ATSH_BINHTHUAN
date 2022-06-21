/* Chương trình giao tiếp API hiển thi thời gian và phát thông báo */
#include <Wire.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>
#define SERVER_IP "172.17.128.50:58185/api/Farm/getcountdownsecond"  //http://172.17.128.50:58185/api/Farm/getcountdownsecond
#ifndef STASSID
#define STASSID "TOTOLINK N300RH"
#define STAPSK  ""
const char* ssid = STASSID;
const char* password = STAPSK;
int timer_uv = 0, gio = 0, phut = 0, giay = 0, dem = 0, load = 0, i = 0, starus = 0, big = 0;
#endif
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("ket noi wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(">");
  }
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print("Failed! Reboot");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  //ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  //   MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  //ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

}
void loop() {
  Serial.println("loop");
  delay(10000);
  gui();

}
void gui() {
  WiFiClient client; HTTPClient http;
  http.begin(client, "http://" SERVER_IP );
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"mac_address\":\"dc:a6:32:0f:bd:ac\"}");//6c:1c:71:5c:9b:31
  if (httpCode > 0) {
    Serial.print(httpCode); delay(250);
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("ok ok ok");
    } else {
      Serial.print("failed");
    }
    http.end();
  }
}
