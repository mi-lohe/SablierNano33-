#include "Math.h"
#include "Gravite.h"
#include "ConstantDefine.h"

void IMUInit()
{
 if (!IMU.begin()) 
  {
   // Serial.println("Echec de connexion à la centrale inertielle...");
    while (1);
  }
  //else
  //Serial.println("Centralle inertielle active.");
}


uint32_t getGravityAngle()
{
  uint32_t alpha;
  float gx,gy,gz;
  IMU.readAcceleration(gx, gy, gz);

  //alpha = PI_D2 * atan2(gx,gy);
  alpha = ceil(fmod(PI_D2 * atan2(gy,-gx) + 1800,3600)) ;

  return alpha ;
}
