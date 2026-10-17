#include <ESP32Servo.h>
#include "wifi.h"          
#include "kinematics.h"    
#include "pathPlanner.h"


const int servoNum = 10;
int servoPin[servoNum] = {13, 19, 21, 22, 23, 25, 26, 27, 32, 33}; 
Servo myServo[servoNum]; 


volatile float targetX = 5.0; 
volatile float targetY = 0.0;
volatile float targetZ = 9.0;
volatile int robotMode = 0;
volatile float presentX = 5.0; 
volatile float presentY = 0.0;
volatile float presentZ = 9.0;
volatile float presentorientation = 0.0;

WebServer server(80); 
unsigned long webStepStartTime = 0;
enum State{
 idle,
 calculating,
 executing,
 emergency

};
 
 volatile State currentState = idle; 

 
extern int stepstotake;
extern float pathLength;
extern float stepPath;
extern float orientation;
extern float angleerror;

 
void KinematicsLoop(void * pvParameters) {
  vTaskDelay(pdMS_TO_TICKS(500)); 
  Serial.println("loopke started");

  while(1) {
    switch (currentState) {
      case idle:
       moveLeg(-1, FL_THIGH, FL_CALF, 0.0,0.0 ,7.0);//hip
        moveLeg(-1,     RR_THIGH, RR_CALF,  0.0,0.0,  7.0);  
        delayMicroseconds(400); 
        moveLeg(-1, FR_THIGH, FR_CALF,  0.0, 0.0, 7.0); //hip
        moveLeg(-1,     RL_THIGH, RL_CALF,  0.0,0.0, 7.0);  
         delayMicroseconds(400); 
        if (sqrt((targetX - presentX) * (targetX - presentX) + (targetY - presentY) * (targetY - presentY)) > 1.0) {
          currentState = calculating; 
        
        }
        break;
     
      case calculating:
        
        orientation = atan2((targetY - presentY), (targetX - presentX));
        pathLength = sqrt((targetX - presentX) * (targetX - presentX) + (targetY - presentY) * (targetY - presentY));
        stepstotake = round(pathLength / stepPath);
        angleerror = orientation - presentorientation;
         Serial.println(pathLength);
        if (stepstotake > 0) {
          currentState = executing;
        } else {
          currentState = idle;
        }
        break;  

      case executing:
       
        walkstep(angleerror, stepstotake);   
        break;

      case emergency:
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(10)); 
  }
}

void setup() {
  Serial.begin(115200);
  delay(10000); 
  Serial.println("--- Starting Quadruped  ---");

  for(int i = 0; i < servoNum; i++) {
    ESP32PWM::allocateTimer(i);
    myServo[i].setPeriodHertz(50);
    myServo[i].attach(servoPin[i], 500, 2400);
  }
  Serial.println("All servos setup successfully.");

  setupWiFi(); 
  Serial.println("WiFi Web Server Ready.");

  xTaskCreatePinnedToCore(
    KinematicsLoop,   
    "Kinematics",     
    10000,            
    NULL,             
    1,                
    NULL,             
    0                 
  );
  Serial.println("Kinematics engine successfully pinned to Core 0.");
}

void loop() {
  server.handleClient();
}
