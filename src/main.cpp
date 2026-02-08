#include <ESP8266WiFi.h>
#include <stdlib.h>

#define LEFT_BTN 0
#define RIGHT_BTN 2
#define FORWARD_BTN 1
#define BACKWARD_BTN 3
#define DEBUG 0
#define WIFI_AP_MODE 1


#if DEBUG
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x) 
#define debugln(x) 
#endif

int port = 65432;

#if WIFI_AP_MODE
const char *hostIP = "192.168.4.1";
const char *ssid = "ESPESP";
const char *password = "g0T$0m3toD0?";
#else
const char *hostIP = "192.168.1.105";
const char *ssid = "";
const char *password = "";
#endif


bool leftBtn = 0;
bool rightBtn = 0;
bool forwardBtn = 0;
bool backwardBtn = 0;
bool leftBtnPressed_flag = 0;
bool rightBtnPressed_flag = 0;
bool forwardBtnPressed_flag = 0;
bool backwardBtnPressed_flag = 0;
bool valsUpdated_flag = 0;

char boatDirection = 19;
char motorDriveSpeed = 0;
char motorDriveDirection = 1;
char feederState = 0;
char lightsState = 0;
char gpsState = 0;

String setts = "";
String regs = "";

int msgTime = 0;
int msgInterval = 3000;


//=======================================================================
//                    Power on setup
//=======================================================================
void setup() 
{
  #if DEBUG
    Serial.begin(115200);
  #endif

  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);
  pinMode(FORWARD_BTN, INPUT_PULLUP);
  pinMode(BACKWARD_BTN, INPUT_PULLUP);

  WiFi.setOutputPower(20.5);

    WiFi.mode(WIFI_STA);
    //Connect to wifi
    WiFi.begin(ssid, password);

    // Wait for connection  
    debugln("Connecting to Wifi");
    while (WiFi.status() != WL_CONNECTED)
    {   
      delay(500);
      debug(".");
      delay(500);
    }
    debugln("");
    debug("Connected to ");
    debugln(ssid);

}
//=======================================================================
//                    Loop
//=======================================================================

void loop() 
{
  WiFiClient client;
  
  if (!client.connect(hostIP, port)) {
    debugln("connection failed");
    debugln("wait 5 sec...");
    delay(3000);
  }
  
  if (client)
  {
    if(client.connected())
    {
      debugln("Connected to host");
      msgTime = millis();
    }
    
    while(client.connected())
    {     
      leftBtn = digitalRead(LEFT_BTN);
      rightBtn = digitalRead(RIGHT_BTN);
      forwardBtn = digitalRead(FORWARD_BTN);
      backwardBtn = digitalRead(BACKWARD_BTN);
      
      if(leftBtn == 0 && leftBtnPressed_flag == 0)
      {
        leftBtnPressed_flag = 1;
        boatDirection = 29 ; //26;
        valsUpdated_flag = 1;
      }
      
      if(leftBtn == 1 && leftBtnPressed_flag == 1)
      {
        leftBtnPressed_flag = 0;
        boatDirection = 19;
        valsUpdated_flag = 1;
      }




      if(rightBtn == 0 && rightBtnPressed_flag == 0)
      {
        rightBtnPressed_flag = 1;
        boatDirection = 9; //12;
        valsUpdated_flag = 1;
      }
      
      if(rightBtn == 1 && rightBtnPressed_flag == 1)
      {
        rightBtnPressed_flag = 0;
        boatDirection = 19;
        valsUpdated_flag = 1;
      }



      if(forwardBtn == 0 && forwardBtnPressed_flag == 0)
      {
        forwardBtnPressed_flag = 1;
        motorDriveSpeed = 1;
        motorDriveDirection = 1;
        valsUpdated_flag = 1;
      }
      
      if(forwardBtn == 1 && forwardBtnPressed_flag == 1)
      {
        forwardBtnPressed_flag = 0;
        motorDriveSpeed = 0;
        valsUpdated_flag = 1;
      }



      if(backwardBtn == 0 && backwardBtnPressed_flag == 0)
      {
        backwardBtnPressed_flag = 1;
        motorDriveSpeed = 1;
        motorDriveDirection = 0;
        valsUpdated_flag = 1;
      }
      
      if(backwardBtn == 1 && backwardBtnPressed_flag == 1)
      {
        backwardBtnPressed_flag = 0;
        motorDriveSpeed = 0;
        motorDriveDirection = 1;
        valsUpdated_flag = 1;
      }


      if(valsUpdated_flag == 1 || (millis() >= msgTime + msgInterval))
      {
        valsUpdated_flag = 0;
        char buff[5];
        String temp;
        
        setts = itoa(feederState, buff, 2);
        setts += itoa(motorDriveSpeed, buff, 2);
        setts += itoa(motorDriveDirection, buff, 2);
        temp = itoa(boatDirection, buff, 2);
        temp.length() < 5 ? temp = "0" + temp : temp;
        setts += temp;
        
        sprintf(buff, "%02X", strtoul(setts.c_str(), NULL, 2));
        setts = buff;
        setts += "h";
        
        client.print(setts);
        
        msgTime = millis();
      }

      delay(5);
      
    }
    client.stop();
    debugln("Client disconnected");    
  }
}