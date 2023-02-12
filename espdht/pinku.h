#include "key.h"

int tMin=-50,tMax=50;//ButtonMin Max

const char* ssid = yourSSID ;
const char* pass = yourPASS ;
String devID=yourDeviceID;

#define sendDevThresholdURL "https://iot.partnerkita.com/setThreshold.php"
#define sendDevDHTURL "https://iot.partnerkita.com/setDHT.php"
#define readWebThresholdURL "https://iot.partnerkita.com/getThreshold.php"

#define buttonPinU 32 //Up
#define buttonPinD 25
#define buttonPinOK 33

#define myDHT1 17
#define myDHT2 16
#define relayPin 18
