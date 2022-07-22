// Tested and compiled with no errors
// measuring AC current using ACS712 current sensor with ESP32 Microcontroller
// The ACS712 works with high voltage AC so be careful !
// source - /www.circuitschools.com
 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); //Thiết lập địa chỉ và loại LCD
const int sensorIn = 4;      // pin where the OUT pin from sensor is connected on Arduino
int mVperAmp = 185;           // this the 5A version of the ACS712 -use 100 for 20A Module and 66 for 30A Module
int Watt = 0;
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

unsigned long time1 = 0;
unsigned long time2 = 0;

byte TRIAC3 = 16;
byte LED = 2;
 
void setup() {
  
  Serial.begin (115200); 
  Wire.begin(); 
  lcd.init();                      //Khởi tạo LCD
  lcd.backlight();                 //Bật đèn nền
  lcd.setCursor(0, 0);
  Serial.println ("ACS712 current sensor");  
  lcd.print ("ACS712 currentsensor"); 
  pinMode(TRIAC3, OUTPUT);
  pinMode(LED, OUTPUT);
  delay(2000);
  lcd.clear();
}
 
void loop() {
  lcd.setCursor(0, 0);
  Serial.println (""); 
  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;   //root 2 is 0.707
  AmpsRMS = ((VRMS * 1000)/mVperAmp)-0.3; //0.3 is the error I got for my sensor
 
  Serial.print(AmpsRMS);
  lcd.print(AmpsRMS);
  Serial.print(" Amps RMS  ---  ");
  lcd.print(" Amps RMS ");
  Watt = (AmpsRMS*240/1.2);
  // note: 1.2 is my own empirically established calibration factor
// as the voltage measured at A0 depends on the length of the OUT-to-A0 wire
// 240 is the main AC power voltage – this parameter changes locally
  lcd.setCursor(0, 1);
  Serial.print(Watt);
  lcd.print(Watt);
  Serial.println(" Watts");
  lcd.print(" Watts");
  if ( digitalRead(TRIAC3) == 1 )
  {
    Serial.println(" LED 220v ON ");
  } else {
    Serial.println(" LED 220v OFF ");
  }
  delay (1000);

  if ( (unsigned long) (millis() - time1) > 100000 )
    {
        if ( digitalRead(TRIAC3) == LOW )
        {
            digitalWrite(TRIAC3, HIGH);
        } else {
            digitalWrite(TRIAC3, LOW );
        }
        time1 = millis();
    }
    
    if ( (unsigned long) (millis() - time2) > 200  )
    {
        if ( digitalRead(LED) == LOW )
        {
            digitalWrite(LED, HIGH);
        } else {
            digitalWrite(LED, LOW );
        }
        time2 = millis();
    }
    
}
 
// ***** function calls ******
float getVPP()
{
  float result;
  int readValue;                // value read from the sensor
  int maxValue = 0;             // store max value here
  int minValue = 4096;          // store min value here ESP32 ADC resolution
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5)/4096.0; //ESP32 ADC resolution 4096
      
   return result;
 }
 
