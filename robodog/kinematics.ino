#include "kinematics.h"
#include <Arduino.h>
extern Servo myServo[];
// --- Function Body (The Construction Work) ---
void moveLeg(int hipIdx, int thighIdx, int calfIdx, float x, float y, float z) {
  float theta1, theta2, theta3;

  // float r1 = sqrt(y * y + z * z);
  // float target_rad = (r1 * r1) - (ry * ry);
  // if (target_rad < 0) target_rad = 0;

  // float r23 = sqrt(target_rad) - rz;
  // float b1 = atan2(y, z);

  // float ratio = ry / r1;
  // if (ratio > 1.0) ratio = 1.0;
  // if (ratio < -1.0) ratio = -1.0;
  // float b2 = acos(ratio);

  // theta3 = PI_VAL - b1 - b2;
   float t1 = 5.5;   // Upper thigh length (cm)
   float lg1 = 7.0; 


   switch (calfIdx) {

    case 1:
      t1=5.5;
      lg1=8.0;
      break;
    case 3:
       t1=4.5;
      lg1=7.4;
      break;
    case 5:
       t1=5.2;
      lg1=7.5;
      break;
    case 7:
       t1=5.6;
      lg1=7.4;
      
      break;
  }

   float local_z =z; //r23;
  float h_sq = (x * x) + (local_z * local_z);
  float h = sqrt(h_sq);

  float cos_theta2 = (t1 * t1 + lg1 * lg1 - h_sq) / (2.0 * t1 * lg1);
  if (cos_theta2 > 1.0) cos_theta2 = 1.0;
  if (cos_theta2 < -1.0) cos_theta2 = -1.0;
  theta2 = PI_VAL - acos(cos_theta2);

  float cos_alpha = (t1 * t1 - lg1 * lg1 + h_sq) / (2.0 * t1 * h);
  if (cos_alpha > 1.0) cos_alpha = 1.0;
  if (cos_alpha < -1.0) cos_alpha = -1.0;
  
float angle_to_foot = atan2(local_z, x);
if (angle_to_foot < 0) { angle_to_foot += 2.0 * PI_VAL; }

  theta1 = PI_VAL - angle_to_foot- acos(cos_alpha);

  float angleThigh = theta1 * 180.0 / PI_VAL;
  float angleCalf = theta2 * 180.0 / PI_VAL;
  float angleHip = theta3 * 180.0 / PI_VAL;

   switch (calfIdx) {

    case 1:
      if (angleCalf > 130) { angleCalf = 130; }   
      if (angleCalf < 60) { angleCalf = 60; }
      angleThigh = 80- angleThigh  ;
      break;
    case 3:
      if (angleCalf > 130) { angleCalf = 130; }
      if (angleCalf < 60) { angleCalf = 60; }  //fr//rf
      angleCalf =180- angleCalf;
      angleThigh = 90+angleThigh;
      break;
    case 5:
      if (angleCalf > 130) { angleCalf = 130; } //lr/rl
      if (angleCalf < 60) { angleCalf = 60; }
      angleCalf = 3+ angleCalf;
      angleThigh = 90-angleThigh ;
      break;
    case 7:
     if (angleCalf > 140) { angleCalf = 140; }
      if (angleCalf < 60) { angleCalf = 60; }  //rr//rr
      angleCalf = 180 - angleCalf;
      angleThigh = 90+angleThigh;
      
      break;
  }






  if (hipIdx >= 0) {
    myServo[hipIdx].write(angleHip);
  }
  myServo[thighIdx].write(angleThigh);
  myServo[calfIdx].write(angleCalf);
}
