/*Firmansyah Wahyu @2023
  Github : https://github.com/bywahjoe/SimpleControllerDHT

*/

#include "pinku.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define buttonU digitalRead(buttonPinU)
#define buttonD digitalRead(buttonPinD)
#define buttonOK digitalRead(buttonPinOK)
#define relayON digitalWrite(relayPin,HIGH)
#define relayOFF digitalWrite(relayPin,LOW)

LiquidCrystal_I2C lcd(0x27, 16, 2);

int threshold = 25; //Set Default in 25
int newThreshold = 0;
float myTemp1, myTemp2;

//Millis Check Web Server
unsigned long interval = 15000, before = 0, nows = 0;

byte charTemp[8] =
{
  0b00100,
  0b01110,
  0b01010,
  0b01110,
  0b01010,
  0b01110,
  0b11111,
  0b11111
};
byte charLimit[8] =
{
  0b00011,
  0b00110,
  0b01100,
  0b11111,
  0b11111,
  0b00011,
  0b00110,
  0b01100
};
void setup() {
  Serial.begin(115200);
  //Pin Setting
  pinMode(buttonU, INPUT_PULLUP);
  pinMode(buttonD, INPUT_PULLUP);
  pinMode(buttonOK, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);

  //LCD Starting
  lcd.init();
  lcd.createChar(0, charTemp);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFI Connecting..");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };
  lcd.clear();

  readDataWeb();
  nows = millis();
  before = millis();
}

void loop() {
  nows = millis();

  //Read DHT
  myTemp1 = getTemp1();
  myTemp2 = getTemp2();

  //
  if (!buttonOK) {
    newThreshold = threshold;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Threshold:");
    lcd.setCursor(0, 1);
    lcd.print("UP - DOWN - OK");
    delay(1000);
    adjust();
  }

  //Relay Trigger
  if (myTemp1 > threshold || myTemp2 > threshold) {
    relayON;
  }
  else {
    relayOFF;
  }

  //Check Web Interval
  if (nows - before > interval) {
    readDataWeb();  //GetThresholdWeb
    before = millis();
  }
}
float getTemp1() {
  float val;
  return val;
}
float getTemp2() {
  float val;
  return val;
}
void viewDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); lcd.print(myTemp1);
  lcd.setCursor(0, 1);
  lcd.write(byte(0)); lcd.print(myTemp1);

  lcd.setCursor(12, 0);
  lcd.write(byte(0)); lcd.print(threshold);
  delay(1000);

}
void readDataWeb() {
  //This read from last threshold API

}
void sendDataWeb() {
  String d1 = String(threshold);
  //This push data new threshold to WEB
  HTTPClient postWeb;

  postWeb.begin(pushURL);
  postWeb.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String dataku = "apiKEY=" + APIKEY + "&d1=" + d1;
  Serial.println(dataku);

  int httpResponseCode = postWeb.POST(dataku);

  //  UNCOMENT TO CEK ERROR
  //     if (httpResponseCode>0) {
  //        Serial.print("HTTP Response code: ");
  //        Serial.println(httpResponseCode);
  //      }
  //      else {
  //        Serial.print("Error code: ");
  //        Serial.println(httpResponseCode);
  //      }
  postWeb.end();
}
void adjust() {
  bool main = true;

  while (main) {
    lcd.setCursor(10, 0);
    lcd.print(newThreshold);

    //0-50
    //Up
    if (!buttonU) {
      lcd.clear();
      delay(150);
      if ((newThreshold + 1) < 50)newThreshold = newThreshold + 1;
    }

    //Down
    if (!buttonD) {
      lcd.clear();
      delay(150);
      if ((newThreshold - 1) > 0)newThreshold = newThreshold - 1;
    }

    //Ok-Confirm
    if (!buttonOK) {
      lcd.clear();
      lcd.setCursor(10, 0);
      lcd.print("OK");
      delay(150);
      sendDataWeb();
      threshold = newThreshold;
      main = false;
    }
  }
}
