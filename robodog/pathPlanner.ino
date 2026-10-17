#include "pathPlanner.h"

//  (Points to variables owned by main.ino) ---
extern volatile float targetX;
extern volatile float targetY;
extern volatile float targetZ;
extern volatile int robotMode; 
extern volatile float presentX;
extern volatile float presentY;
extern volatile float presentZ;
extern volatile float presentorientation;
extern volatile State currentState; // DO NOT add "= idle" here! Main owns it.

// Timing Constants ---
unsigned long stepStartTime = 0; 
const unsigned long strideDuration = 400; 


float stepPath = 3.0;       
float stepheight = 1.0;     
int stepstotake = 0;        
int stepscompleted = 0;     
bool isswingphase = true;   
bool stepphase = false;     
float diagonal = 1;         

//  Calculations ---
float pathLength = 0;
float orientation = 0;
float angleerror = 0;
const float pi = 3.141592;
 
void walkstep(float errorAngle, int totalSteps) {
 
  if (abs(errorAngle) > 0.08 && abs(errorAngle) < 3.05) { 
    
    float OST = 4.0; 
    float OSH = 1.0; 
    
    unsigned long timeElapsedLocal = millis() - stepStartTime;
    float progress = (float)timeElapsedLocal / (float)strideDuration;
    float smoothProgress = (1.0 - cos(pi * progress)) / 2.0;
    int directionfactor = (errorAngle > 0) ? 1 : -1;

    if (timeElapsedLocal >= strideDuration) {
      stepStartTime = millis();
      isswingphase = !isswingphase;
      if (isswingphase) stepphase = true; 
    }

    if (stepphase) {
      presentorientation += (0.05 * directionfactor); 
      stepphase = false;
      diagonal = !diagonal; 
    }

    if (isswingphase) {
      float swingX = OST * smoothProgress * directionfactor;
      float swingZ = 7.0 - OSH * sin(pi * smoothProgress);

      if (diagonal == 1) {
        moveLeg(-1, FL_THIGH, FL_CALF, -swingX, 0, swingZ);//hip
        moveLeg(-1,     RR_THIGH, RR_CALF,  swingX, 0, swingZ);  
        delayMicroseconds(400); 
        moveLeg(-1, FR_THIGH, FR_CALF,  0, 0, 7.0); //hip
        moveLeg(-1,     RL_THIGH, RL_CALF,  0, 0, 7.0);  
      } else {
        moveLeg(-1, FR_THIGH, FR_CALF,  swingX, 0, swingZ); //hip
        moveLeg(-1,     RL_THIGH, RL_CALF, -swingX, 0, swingZ);  
        delayMicroseconds(400); 
        moveLeg(-1, FL_THIGH, FL_CALF,  0, 0, 7.0);//hip
        moveLeg(-1,     RR_THIGH, RR_CALF,  0, 0, 7.0);
      }
    } 
    else { 
      float stanceX = OST * (1.0 - smoothProgress) * directionfactor;

      if (diagonal == 1) {
        moveLeg(-1, FL_THIGH, FL_CALF, -stanceX, 0, 7.0);
        moveLeg(-1,     RR_THIGH, RR_CALF,  stanceX, 0, 7.0);
        delayMicroseconds(400); 
        moveLeg(-1, FR_THIGH, FR_CALF,  0, 0, 7.0);
        moveLeg(-1,     RL_THIGH, RL_CALF,  0, 0, 7.0);
      } else {
        moveLeg(-1, FR_THIGH, FR_CALF,  stanceX, 0, 7.0);
        moveLeg(-1,     RL_THIGH, RL_CALF, -stanceX, 0, 7.0);
        delayMicroseconds(400); 
        moveLeg(-1, FL_THIGH, FL_CALF,  0, 0, 7.0);
        moveLeg(-1,     RR_THIGH, RR_CALF,  0, 0, 7.0);
      }
    }
  } 
  
  
  else {
    unsigned long timeElapsed = millis() - stepStartTime;
    float progress = (float)timeElapsed / (float)strideDuration;
    float smoothProgress = (1.0 - cos(pi * progress)) / 2.0;
    if (timeElapsed >= strideDuration) {
      stepStartTime = millis();
      isswingphase = !isswingphase;
      if (!isswingphase) stepphase = true; 
    }

    if (stepphase) {
      stepscompleted++;
      presentX += stepPath * cos(presentorientation);
      presentY += stepPath * sin(presentorientation); 
      stepphase = false;
      diagonal = !diagonal; 

      if (stepscompleted >= totalSteps) {
        presentX = targetX;
        presentY = targetY;
        
        currentState = idle; 
        Serial.print("reached");
        stepscompleted = 0;
        return;
      }
    }

    int walkDirection = (cos(errorAngle) > 0) ? 1 : -1;

    if (isswingphase) {
      float swingX = ((stepPath * smoothProgress) - (stepPath / 2.0)) * walkDirection;
      float swingZ = 7.0 - stepheight * sin(pi * smoothProgress);

      if (diagonal == 1) {
        moveLeg(-1, FL_THIGH, FL_CALF, swingX, 0, swingZ); 
        moveLeg(-1,     RR_THIGH, RR_CALF, swingX, 0, swingZ); 
        delayMicroseconds(400);
        moveLeg(-1, FR_THIGH, FR_CALF, 0, 0, 7.0);
        moveLeg(-1,     RL_THIGH, RL_CALF, 0, 0, 7.0);
      } else {
        moveLeg(-1, FR_THIGH, FR_CALF, swingX, 0, swingZ); 
        moveLeg(-1,     RL_THIGH, RL_CALF, swingX, 0, swingZ); 
        delayMicroseconds(400);
        moveLeg(-1, FL_THIGH, FL_CALF, 0, 0, 7.0);
        moveLeg(-1,     RR_THIGH, RR_CALF, 0, 0, 7.0);
      }
    } 
    else { 
      float stanceX = ((stepPath / 2.0) - (stepPath * smoothProgress)) * walkDirection;

      if (diagonal == 1) {
        moveLeg(-1, FL_THIGH, FL_CALF, stanceX, 0, 7.0);
        moveLeg(-1,     RR_THIGH, RR_CALF, stanceX, 0, 7.0);
        delayMicroseconds(400);
        moveLeg(-1, FR_THIGH, FR_CALF, 0, 0, 7.0);
        moveLeg(-1,     RL_THIGH, RL_CALF, 0, 0, 7.0);
      } else {
        moveLeg(-1, FR_THIGH, FR_CALF, stanceX, 0, 7.0);
        moveLeg(-1,     RL_THIGH, RL_CALF, stanceX, 0, 7.0);
        delayMicroseconds(400);
        moveLeg(-1, FL_THIGH, FL_CALF, 0, 0, 7.0);
        moveLeg(-1,     RR_THIGH, RR_CALF, 0, 0, 7.0);
      }
    }
  }
}
