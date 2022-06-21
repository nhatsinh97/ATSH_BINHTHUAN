#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> //v5.13.5

const char* ssid = "GreenFeed";
const char* password = "2020@GREENFEED";


void setup() {
  Serial.begin(115200);
  delay(4000);
  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  Serial.println("Connected to the WiFi network");


}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    http.begin("http://192.168.32.239:58888/api/refresh_service"); //My docker
    http.addHeader("Content-Type", "application/json");

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["id"] = "urn:ngsi-ld:Sensor:001";
    root["type"] = "motion";
    root["value"] = "No";
    root.printTo(Serial);
    /*int httpResponseCode = http.POST("{\n\t\"id\":\"urn:ngsi-ld:Sensor:001\", \"type\":\"MotionSensor\",\n\t\"value\":\"NO\"\n}"); */
    int httpResponseCode = http.POST(root);
    if(httpResponseCode > 0){
      String response = http.getString();

      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else{
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else{
    Serial.println("Error in WiFi connection");
  }
  delay(300000);
}
