/*Firmansyah Wahyu @2023
  Github : https://github.com/bywahjoe/SimpleControllerDHT

*/

#include "pinku.h"
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define buttonU digitalRead(buttonPinU)
#define buttonD digitalRead(buttonPinD)
#define buttonOK digitalRead(buttonPinOK)
#define relayON digitalWrite(relayPin,HIGH)
#define relayOFF digitalWrite(relayPin,LOW)

StaticJsonDocument<200> doc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht1(myDHT1, DHT11);
DHT dht2(myDHT2, DHT11);

//Threshold
int threshold = 25; //Set Default in 25
int minDHT1 = -10,maxDHT1 = 50; 
int minDHT2 = -20,maxDHT2 = 40; 

int newThreshold = 0;
float myTemp1, myTemp2;

//Millis Setting
unsigned long nows = 0;
unsigned long intervalSendDHT = 15000, beforeDHT = 0;
unsigned long intervalFromWEB = 25000, beforeWEB = 0;
unsigned long intervalCON = 30000, beforeCON = 0;
bool errCon = false;
bool isRelayON = false;

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
byte charBlock[8] =
{
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
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
  lcd.createChar(2, charBlock);

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
//  parseJSON();
//  while(1){}///On Going Tes JSON
  lcd.setCursor(0, 0);
  lcd.print("Read Config.");
  getWEBThreshold();
  lcd.clear();

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
    menu();
    resetMillis();
    //    adjust();

  }

  //Relay Trigger
  if (myTemp1 > maxDHT1 || myTemp2 > maxDHT2) {
    relayON;
    isRelayON = true;
  }
  else if (myTemp1 < minDHT1 || myTemp2 < minDHT2) {
    relayOFF;
    isRelayON = false;
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
void menu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LOADING MENU...");
  while (!buttonOK)delay(50);
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("<<--  MENU  ++>>");
  bool play = true;
  bool selected = false;
  int counter = 1;
  int menuMin = 1, menuMax = 5;
  unsigned long timer, lastTimer, blinkTimer = 300;
  while (play) {
    Serial.print("Counter:"); Serial.println(counter);
    if (!buttonU) {
      if (counter < menuMax) {
        counter = counter + 1;
        while (!buttonU);
        clearLine(0);
      }
    }
    if (!buttonD) {
      if (counter > menuMin) {
        counter = counter - 1;
        while (!buttonD);
        clearLine(0);
      }
    }
    if (!buttonOK) {
      selected = true;
      while (!buttonOK);
      timer = millis();
      lastTimer = millis();
    }
    switch (counter) {
      case 1: //RelayOFF MIN DHT1
        lcd.setCursor(0, 0);
        lcd.print("S1"); lcd.write(byte(2)); lcd.print("Set OFF <");
        lcd.setCursor(13, 0);
        lcd.print(minDHT1);
        while (selected) {
          timer = millis();
          lcd.setCursor(13, 0);
          lcd.print(minDHT1);
          if (timer - lastTimer >= blinkTimer) {
            lcd.setCursor(13, 0);
            lcd.print("   ");
            delay(25);
            lastTimer = millis();
          }
          if (!buttonU) {
            while (!buttonU);
            if (minDHT1 > tMin && minDHT1 < tMax)minDHT1 = minDHT1 + 1;
          }
          if (!buttonD) {
            while (!buttonD);
            if (minDHT1 > tMin && minDHT1 < tMax)minDHT1 = minDHT1 - 1;
          }
          if (!buttonOK) {
            saveOK(1);
            play = false; selected = false;
          }
        };
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("S1"); lcd.write(byte(2)); lcd.print("Set ONN >");
        lcd.setCursor(13, 0);
        lcd.print(maxDHT1);
        while (selected) {
          timer = millis();
          lcd.setCursor(13, 0);
          lcd.print(maxDHT1);
          if (timer - lastTimer >= blinkTimer) {
            lcd.setCursor(13, 0);
            lcd.print("   ");
            delay(25);
            lastTimer = millis();
          }
          if (!buttonU) {
            while (!buttonU);
            if (maxDHT1 > tMin && maxDHT1 < tMax)maxDHT1 = maxDHT1 + 1;
          }
          if (!buttonD) {
            while (!buttonD);
            if (maxDHT1 > tMin && maxDHT1 < tMax)maxDHT1 = maxDHT1 - 1;
          }
          if (!buttonOK) {
            saveOK(1);
            play = false; selected = false;
          }
        };
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("S2"); lcd.write(byte(2)); lcd.print("Set OFF <");
        lcd.setCursor(13, 0);
        lcd.print(minDHT2);
        while (selected) {
          timer = millis();
          lcd.setCursor(13, 0);
          lcd.print(minDHT2);
          if (timer - lastTimer >= blinkTimer) {
            lcd.setCursor(13, 0);
            lcd.print("   ");
            delay(25);
            lastTimer = millis();
          }
          if (!buttonU) {
            while (!buttonU);
            if (minDHT2 > tMin && minDHT2 < tMax)minDHT2 = minDHT2 + 1;
          }
          if (!buttonD) {
            while (!buttonD);
            if (minDHT2 > tMin && minDHT2 < tMax)minDHT2 = minDHT2 - 1;
          }
          if (!buttonOK) {
            saveOK(1);
            play = false; selected = false;
          }
        };
        break;
      case 4:
        lcd.setCursor(0, 0);
        lcd.print("S2"); lcd.write(byte(2)); lcd.print("Set ONN >");
        lcd.setCursor(13, 0);
        lcd.print(maxDHT2);
        while (selected) {
          timer = millis();
          lcd.setCursor(13, 0);
          lcd.print(maxDHT2);
          if (timer - lastTimer >= blinkTimer) {
            lcd.setCursor(13, 0);
            lcd.print("   ");
            delay(25);
            lastTimer = millis();
          }
          if (!buttonU) {
            while (!buttonU);
            if (maxDHT2 > tMin && maxDHT2 < tMax)maxDHT2 = maxDHT2 + 1;
          }
          if (!buttonD) {
            while (!buttonD);
            if (maxDHT2 > tMin && maxDHT2 < tMax)maxDHT2 = maxDHT2 - 1;
          }
          if (!buttonOK) {
            saveOK(1);
            play = false; selected = false;
          }
        };
        break;
      case 5:
        lcd.setCursor(0, 0);
        lcd.print("BACK TO HOME    ");
        if (selected)play = false;
        break;
    }
  }
}
void resetMillis() {
  nows=millis();
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

  //Status Relay
  lcd.setCursor(7, 0);

  if (isRelayON) {
    lcd.print("*");
  }
  else {
    lcd.print(" ");
  };

  //Threshold
  lcd.setCursor(9, 0); lcd.print(minDHT1);
  lcd.setCursor(12, 0); lcd.write(byte(1)); lcd.print(maxDHT1);

  lcd.setCursor(9, 1); lcd.print(minDHT2);
  lcd.setCursor(12, 1); lcd.write(byte(1)); lcd.print(maxDHT2);


  //Con Error
  if (errCon) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!CONNECTION ERR");
    lcd.setCursor(0, 1);
    lcd.print("RESTART DEVICE");
    while (1);
  }
  delay(1000);
}
void parseJSON(){
  unsigned long oke=millis();
  HTTPClient readhttp;
  Serial.println("Read WEB THR");
  String myURL = "http://arduinojson.org/example.json";
  readhttp.begin(myURL);
  int httpResponseCode = readhttp.GET();
  String payload;
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = readhttp.getString();
    Serial.print(payload); Serial.println(" -- NEW THRESHOLD");
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  readhttp.end();

//  
  DeserializationError error = deserializeJson(doc, payload);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  };
  String ok=doc["sensor"];
  Serial.println(ok);
  unsigned long ya=millis();
  Serial.println(ya-oke);
  }
void getWEBThreshold() {
  //This read from last threshold API
  HTTPClient readhttp;
  Serial.println("Read WEB THR");
  String myURL = String(readWebThresholdURL) + "?dev=" + devID;
  readhttp.begin(myURL);
  int httpResponseCode = readhttp.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = readhttp.getString();
    Serial.print(payload); Serial.println(" -- NEW THRESHOLD");
    threshold = payload.toInt();
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
  Serial.print("Sending DHT:"); Serial.print(dht1); Serial.print(" | "); Serial.println(dht2);
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
  Serial.print("Sending Threshold:"); Serial.println(thr);
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
void clearLine(int line) {
  lcd.setCursor(0, line);
  lcd.print("                ");
}
void saveOK(bool newSetting) {
  lcd.clear();
  if (newSetting) {
    lcd.setCursor(0, 0);
    lcd.print("SAVE CONFIG..");
    delay(1000);
  }
}
