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

StaticJsonDocument<200> doc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht1(myDHT1, DHT11);
DHT dht2(myDHT2, DHT11);

char relayMode = 'A';

//Threshold
int minDHT1 = 10, maxDHT1 = 15;
int minDHT2 = 10, maxDHT2 = 15;

int newThreshold = 0;
float myTemp1, myTemp2;
int myHumid1, myHumid2;

//Millis Setting
unsigned long nows = 0;
unsigned long intervalSendDHT = 15000, beforeDHT = 0;
unsigned long intervalFromWEB = 25000, beforeWEB = 0;
unsigned long intervalCON = 30000, beforeCON = 0;

bool errCon = false;
bool isRelayON = false;
bool useWifi = false;

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
byte charArrow[8] =
{
  0b00100,
  0b01110,
  0b11111,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00100
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
  lcd.createChar(3, charArrow);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);

  if (useWifi) {
    lcd.print("WiFI Connect..");
    lcd.setCursor(0, 1);
    lcd.print(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    };
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Read Config.");
    getWEBThreshold();
  }
  else {
    lcd.print("Bypass WiFi");
    lcd.setCursor(0, 1);
    lcd.print("Reset-Config.");
    delay(2000);
  }

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
  myHumid1 = getHumid1();
  myHumid2 = getHumid2();

  viewDisplay();

  //
  if (!buttonOK) {
    menu();
    resetMillis();
  }

  //Relay Trigger
  relayTrig(relayMode);

  if (useWifi) {
    
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
}
int getHumid1() {
  int val = dht1.readHumidity();
  return val;
}
int getHumid2() {
  int val = dht2.readHumidity();
  return val;
}
float getTemp1() {
  float val = dht1.readTemperature();
  return val;
}
float getTemp2() {
  float val = dht2.readTemperature();
  return val;
}
void relayON() {
  digitalWrite(relayPin, HIGH);
  isRelayON = true;
}
void relayOFF() {
  digitalWrite(relayPin, LOW);
  isRelayON = false;
}
void relayTrig(char option) {
  switch (option) {
    case 'A':
      if (myTemp1 > maxDHT1 || myTemp2 > maxDHT2) relayON();
        else if (myTemp1 < minDHT1 || myTemp2 < minDHT2)relayOFF();
      break;
    case 'B':
      if (myTemp1 > maxDHT1 || myTemp2 > maxDHT2) relayOFF();
      else if (myTemp1 < minDHT1 || myTemp2 < minDHT2)relayON();
      break;
    default: break;
  }
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

  bool selectMenu = true;
  bool confirmMenu = false;
  bool setTHR = false;
  int index = 1;
  int indexMin = 1, indexMax = 3;

  while (selectMenu) {
    if (!buttonU) {
      while (!buttonU);
      if (index >= indexMin && index < indexMax)index = index + 1;
    }
    if (!buttonD) {
      while (!buttonD);
      if (index > indexMin && index <= indexMax)index = index - 1;
    }
    if (!buttonOK) {
      while (!buttonOK);
      confirmMenu = true;
    }
    switch (index) {
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("MA"); lcd.write(byte(2)); lcd.print(" HIGH"); lcd.write(byte(3)); lcd.print(" = ON ");
        if (confirmMenu) {
          relayMode = 'A';
          setTHR = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("SET: MODE[A]");
          delay(2000);
          lcd.clear();
        }
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("MB"); lcd.write(byte(2)); lcd.print(" HIGH"); lcd.write(byte(3)); lcd.print(" = OFF");
        if (confirmMenu) {
          relayMode = 'B';
          setTHR = true;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("SET: MODE[B]");
          delay(2000);
          lcd.clear();
        }
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("BACK TO HOME    ");
        if (confirmMenu)selectMenu = false;
        break;
    }

    //Continue to Config
    if (setTHR) {
      configTHR(setTHR);
      selectMenu = false;
    }
  }
}
void configTHR(bool setTHR) {
  lcd.setCursor(0, 0);
  lcd.print("S1"); lcd.write(byte(2)); lcd.print("L1:"); lcd.print(minDHT1);
  lcd.setCursor(10, 0);
  lcd.print("H1:"); lcd.print(maxDHT1);

  lcd.setCursor(0, 1);
  lcd.print("S2"); lcd.write(byte(2)); lcd.print("L2:"); lcd.print(minDHT2);
  lcd.setCursor(10, 1);
  lcd.print("H2:"); lcd.print(maxDHT2);
  unsigned long timer = millis(), lastTimer = millis(), blinkTimer = 300;
  int start = 1;
  while (setTHR) {
    timer = millis();
    switch (start) {
      case 1:
        lcd.setCursor(6, 0);
        lcd.print(minDHT1);
        lcd.setCursor(6, 0);
        if (!buttonU) {
          while (!buttonU);
          if (minDHT1 >= tMin && minDHT1 < tMax)minDHT1 = minDHT1 + 1;
        }
        if (!buttonD) {
          while (!buttonD);
          if (minDHT1 > tMin && minDHT1 <= tMax)minDHT1 = minDHT1 - 1;
        };
        break;
      case 2:
        lcd.setCursor(13, 0);
        lcd.print(maxDHT1);
        lcd.setCursor(13, 0);
        if (!buttonU) {
          while (!buttonU);
          if (maxDHT1 >= tMin && maxDHT1 < tMax)maxDHT1 = maxDHT1 + 1;
        }
        if (!buttonD) {
          while (!buttonD);
          if (maxDHT1 > tMin && maxDHT1 <= tMax)maxDHT1 = maxDHT1 - 1;
        };
        break;
      case 3:
        lcd.setCursor(6, 1);
        lcd.print(minDHT2);
        lcd.setCursor(6, 1);
        if (!buttonU) {
          while (!buttonU);
          if (minDHT2 >= tMin && minDHT2 < tMax)minDHT2 = minDHT2 + 1;
        }
        if (!buttonD) {
          while (!buttonD);
          if (minDHT2 > tMin && minDHT2 <= tMax)minDHT2 = minDHT2 - 1;
        };
        break;
      case 4:
        lcd.setCursor(13, 1);
        lcd.print(maxDHT2);
        lcd.setCursor(13, 1);
        if (!buttonU) {
          while (!buttonU);
          if (maxDHT2 >= tMin && maxDHT2 < tMax)maxDHT2 = maxDHT2 + 1;
        }
        if (!buttonD) {
          while (!buttonD);
          if (maxDHT2 > tMin && maxDHT2 <= tMax)maxDHT2 = maxDHT2 - 1;
        };
        break;
      case 5:
        setTHR = false;
        saveOK();
        break;
    };
    if (!buttonOK) {
      while (!buttonOK)delay(50);
      start = start + 1;
      delay(1000);
    } else {
      if (timer - lastTimer >= blinkTimer) {
        lcd.print("   ");
        delay(25);
        lastTimer = millis();
      }
    }
  }
}
void resetMillis() {
  nows = millis();
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

  //Status MODE Relay
  lcd.setCursor(7, 1);
  lcd.print(relayMode);

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
void getWEBThreshold() {
  HTTPClient readhttp;
  Serial.println("Read WEB THR");
  String myURL = String(readWebThresholdURL) + "?dev=" + devID;

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
  String ok = doc["mode"];
  relayMode = ok[0];
  minDHT1 = doc["dht1"]["minDHT1"];
  maxDHT1 = doc["dht1"]["maxDHT1"];
  minDHT2 = doc["dht2"]["minDHT2"];
  maxDHT2 = doc["dht2"]["maxDHT2"];
}
void sendDevDHT() {
  String dht1 = String(myTemp1);
  String dht2 = String(myTemp2);
  String humid1 = String(myHumid1);
  String humid2 = String(myHumid2);

  Serial.print("Sending DHT:"); Serial.print(dht1); Serial.print(" | "); Serial.println(dht2);
  Serial.print("Sending HUMID:"); Serial.print(humid1); Serial.print(" | "); Serial.println(humid2);

  HTTPClient postWeb;
  postWeb.begin(sendDevDHTURL);
  postWeb.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String dataku = "dev=" + devID + "&dht1=" + dht1 + "&dht2=" + dht2 + "&humid1=" + humid1 + "&humid2=" + humid2;

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
  String md = String(relayMode);
  String min1 = String(minDHT1);
  String max1 = String(maxDHT1);
  String min2 = String(minDHT2);
  String max2 = String(maxDHT2);

  Serial.print("Sending Threshold:");
  Serial.print(md); Serial.print(",");
  Serial.print(min1); Serial.print(",");
  Serial.print(max1); Serial.print(",");
  Serial.print(min2); Serial.print(",");
  Serial.println(max2);

  HTTPClient postWeb;
  postWeb.begin(sendDevThresholdURL);
  postWeb.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String dataku = "dev=" + devID + "&mode=" + md + "&min1=" + min1 + "&max1=" + max1 + "&min2=" + min2 + "&max2=" + max2;

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
void clearLine(int line) {
  lcd.setCursor(0, line);
  lcd.print("                ");
}
void saveOK() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SAVE CONFIG..");
  if(useWifi)sendDevThreshold();
  delay(1000);
}
