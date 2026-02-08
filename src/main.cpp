#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <stdint.h>

#define LEFT_BTN 0
#define RIGHT_BTN 2
#define FORWARD_BTN 1
#define BACKWARD_BTN 3

#define BTN_PRESSED 0
#define BTN_RELEASED 1

#define BOAT_DIRECTION_STRAIGHT 19
#define BOAT_DIRECTION_LEFT 29
#define BOAT_DIRECTION_RIGHT 9
#define MOTOR_DRIVE_DIRECTION_FORWARD 1
#define MOTOR_DRIVE_DIRECTION_BACKWARD 0
#define MOTOR_RUN 1
#define MOTOR_STOP 0
#define FEEDER_OPEN 1
#define FEEDER_CLOSED 0
#define LIGHTS_ON 1
#define LIGHTS_OFF 0
#define GPS_ON 1
#define GPS_OFF 0


#define DEBUG 0
#define WIFI_AP_MODE 1

#define MESSAGE_INTERVAL 3000UL


#if DEBUG
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x) 
#define debugln(x) 
#endif

uint16_t boatPort = 65432;

#if WIFI_AP_MODE
const char *boatIP = "192.168.4.1";
const char *boatSSID = "FeedBoat";
const char *boatPassword = "g0T$0m3toD0?";
#else
const char *boatIP = "192.168.1.105";
const char *boatSSID = "";
const char *boatPassword = "";
#endif


uint8_t leftBtn = 0;
uint8_t rightBtn = 0;
uint8_t forwardBtn = 0;
uint8_t backwardBtn = 0;

uint8_t leftBtnPressed_flag = false;
uint8_t rightBtnPressed_flag = false;
uint8_t forwardBtnPressed_flag = false;
uint8_t backwardBtnPressed_flag = false;
uint8_t valsUpdated_flag = false;

uint8_t boatDirection = BOAT_DIRECTION_STRAIGHT;
uint8_t motorDriveSpeed = MOTOR_STOP;
uint8_t motorDriveDirection = MOTOR_DRIVE_DIRECTION_FORWARD;
uint8_t feederState = FEEDER_CLOSED;
uint8_t lightsState = LIGHTS_OFF;
//uint8_t gpsState = 0; //Future usage of gps positoning

String setts = "";

uint32_t msgTime = 0;


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
  WiFi.begin(boatSSID, boatPassword);

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
  debugln(boatSSID);

}
//=======================================================================
//                    Loop
//=======================================================================

void loop() 
{
  WiFiClient client;
  
  if (!client.connect(boatIP, boatPort)) {
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
      
      if(leftBtn == BTN_PRESSED && leftBtnPressed_flag == false)
      {
        leftBtnPressed_flag = true;
        boatDirection = BOAT_DIRECTION_LEFT;
        valsUpdated_flag = true;
      }
      
      if(leftBtn == BTN_RELEASED && leftBtnPressed_flag == true)
      {
        leftBtnPressed_flag = false;
        boatDirection = BOAT_DIRECTION_STRAIGHT;
        valsUpdated_flag = true;
      }




      if(rightBtn == BTN_PRESSED && rightBtnPressed_flag == false)
      {
        rightBtnPressed_flag = true;
        boatDirection = BOAT_DIRECTION_RIGHT;
        valsUpdated_flag = true;
      }
      
      if(rightBtn == BTN_RELEASED && rightBtnPressed_flag == true)
      {
        rightBtnPressed_flag = false;
        boatDirection = BOAT_DIRECTION_STRAIGHT;
        valsUpdated_flag = true;
      }



      if(forwardBtn == BTN_PRESSED && forwardBtnPressed_flag == false)
      {
        forwardBtnPressed_flag = true;
        motorDriveSpeed = MOTOR_RUN;
        motorDriveDirection = MOTOR_DRIVE_DIRECTION_FORWARD;
        valsUpdated_flag = true;
      }
      
      if(forwardBtn == BTN_RELEASED && forwardBtnPressed_flag == true)
      {
        forwardBtnPressed_flag = false;
        motorDriveSpeed = MOTOR_STOP;
        valsUpdated_flag = true;
      }



      if(backwardBtn == BTN_PRESSED && backwardBtnPressed_flag == false)
      {
        backwardBtnPressed_flag = true;
        motorDriveSpeed = MOTOR_RUN;
        motorDriveDirection = MOTOR_DRIVE_DIRECTION_BACKWARD;
        valsUpdated_flag = true;
      }
      
      if(backwardBtn == BTN_RELEASED && backwardBtnPressed_flag == true)
      {
        backwardBtnPressed_flag = false;
        motorDriveSpeed = MOTOR_STOP;
        motorDriveDirection = MOTOR_DRIVE_DIRECTION_FORWARD;
        valsUpdated_flag = true;
      }


      if(valsUpdated_flag == true || (millis() >= (msgTime + MESSAGE_INTERVAL)))
      {
        valsUpdated_flag = false;
        char buff[5];
        String temp;
        
        setts = itoa(feederState, buff, 2);
        setts += itoa(motorDriveSpeed, buff, 2);
        setts += itoa(motorDriveDirection, buff, 2);
        temp = itoa(boatDirection, buff, 2);
        temp.length() < 5 ? temp = "0" + temp : temp;
        setts += temp;
        
        sprintf(buff, "%02Xh", (unsigned int)strtoul(setts.c_str(), NULL, 2));
        setts = buff;
        
        client.print(setts);
        client.flush();
        
        msgTime = millis();
      }

      delay(5);
      
    }
    client.stop();
    debugln("Client disconnected");    
  }
}