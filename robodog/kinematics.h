#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <ESP32Servo.h>


const float ry = 2.0;   // Hip offset Y (cm)
const float rz = 1.0;   // Hip offset Z (cm)
const float PI_VAL = 3.141592;

 
#define FL_THIGH 0
#define FL_CALF  1
// #define FR_HIP   3
#define FR_THIGH 2
#define FR_CALF  3
#define RL_THIGH 4
#define RL_CALF  5
#define RR_THIGH 6
#define RR_CALF  7

 
void moveLeg(int hipIdx, int thighIdx, int calfIdx, float x, float y, float z);

#endif
