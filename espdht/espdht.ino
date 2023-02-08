/*Firmansyah Wahyu @2023
  Github : https://github.com/bywahjoe/SimpleControllerDHT

*/

#include "pinku.h"
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define buttonU digitalRead(buttonPinU)
#define buttonD digitalRead(buttonPinD)
#define buttonOK digitalRead(buttonPinOK)
#define relayON digitalWrite(relayPin,HIGH)
#define relayOFF digitalWrite(relayPin,LOW)

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht1(myDHT1, DHT11);
DHT dht2(myDHT2, DHT11);

int threshold = 25; //Set Default in 25
int newThreshold = 0;
float myTemp1, myTemp2;

//Millis Setting
unsigned long nows = 0;
unsigned long intervalSendDHT = 15000, beforeDHT = 0;
unsigned long intervalFromWEB = 25000, beforeWEB = 0;
unsigned long intervalCON = 30000, beforeCON = 0;
bool errCon = false;

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
  pinMode(buttonPinU, INPUT_PULLUP);
  pinMode(buttonPinD, INPUT_PULLUP);
  pinMode(buttonPinOK, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  dht1.begin();
  dht2.begin();

  //LCD Starting
  lcd.init();
  lcd.createChar(0, charTemp);
  lcd.createChar(1, charLimit);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFI Connect..");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  };
  lcd.clear();

  getWEBThreshold();

  //Millis Hit
  nows = millis();
  beforeDHT = millis();
  beforeWEB = millis();
  beforeCON = millis();

}

void loop() {
  nows = millis();

  //Read DHT
  myTemp1 = getTemp1();
  myTemp2 = getTemp2();
  viewDisplay();

  //
  if (!buttonOK) {
    newThreshold = threshold;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Threshold:");
    lcd.setCursor(0, 1);
    lcd.print("UP - DOWN - OK");
    delay(500);
    adjust();
    resetMillis();
  }

  //Relay Trigger
  if (myTemp1 > threshold || myTemp2 > threshold) {
    relayON;
  }
  else {
    relayOFF;
  }

  //DHT Send
  if (nows - beforeDHT > intervalSendDHT) {
    sendDevDHT();
    beforeDHT = millis();
  }

  //Read WEB Threshold
  if (nows - beforeWEB > intervalFromWEB) {
    getWEBThreshold();  //GetThresholdWeb
    beforeWEB = millis();
  }

  //Message Con Error
  if (nows - beforeCON > intervalCON) {
    if (WiFi.status() != WL_CONNECTED) {
      errCon = true;
    } else {
      errCon = false;
    }
    beforeCON = millis();
  }

}
float getTemp1() {
  float val = dht1.readTemperature();;
  return val;
}
float getTemp2() {
  float val = dht2.readTemperature();
  return val;
}
void resetMillis(){
  beforeDHT = millis();
  beforeWEB = millis();
  beforeCON = millis();
  
}
void viewDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0)); lcd.print(myTemp1);
  lcd.setCursor(0, 1);
  lcd.write(byte(0)); lcd.print(myTemp2);

  lcd.setCursor(12, 0);
  lcd.write(byte(1)); lcd.print(threshold);
  lcd.setCursor(12, 1);
  if (errCon) {
    lcd.print("!ERR");
  } else {
    lcd.print("    ");
  }
  delay(1000);
}
void getWEBThreshold() {
  //This read from last threshold API
  HTTPClient readhttp;
  Serial.println("Read WEB THR");
  String myURL=String(readWebThresholdURL)+"?dev="+devID;
  readhttp.begin(myURL);
  int httpResponseCode = readhttp.GET();
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = readhttp.getString();
    Serial.print(payload);Serial.println(" -- NEW THRESHOLD"); 
    threshold=payload.toInt();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  readhttp.end();
}
void sendDevDHT() {
  String dht1 = String(myTemp1);
  String dht2 = String(myTemp2);
  Serial.print("Sending DHT:");Serial.print(dht1);Serial.print(" | ");Serial.println(dht2);
  HTTPClient postWeb;
  postWeb.begin(sendDevDHTURL);
  postWeb.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String dataku = "dev=" + devID + "&dht1=" + dht1 + "&dht2=" + dht2;

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
void sendDevThreshold() {
  String thr = String(threshold);
  Serial.print("Sending Threshold:");Serial.println(thr);
  HTTPClient postWeb;
  postWeb.begin(sendDevThresholdURL);
  postWeb.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String dataku = "dev=" + devID + "&thr=" + thr;

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
      lcd.setCursor(10, 0);
      lcd.print("   ");
      delay(1000);
      if ((newThreshold + 1) <= tMax)newThreshold = newThreshold + 1;
    }

    //Down
    if (!buttonD) {
      lcd.setCursor(10, 0);
      lcd.print("   ");
      delay(1000);
      if ((newThreshold - 1) >= tMin)newThreshold = newThreshold - 1;
    }

    //Ok-Confirm
    if (!buttonOK) {
      lcd.setCursor(13, 0);
      lcd.print("OK");
      delay(150);
      threshold = newThreshold;
      sendDevThreshold();
      main = false;
    }
  }
}
