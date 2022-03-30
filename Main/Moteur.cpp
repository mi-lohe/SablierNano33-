#include "Moteur.h"
#include "Arduino.h"


void stepperInit()
{

  pinMode(PIN_MOT_FDC,INPUT);
  pinMode(PIN_MOT_EN,OUTPUT);
  pinMode(PIN_MOT_DIR,OUTPUT);
 // pinMode(PIN_MOT_STP,OUTPUT);
  digitalWrite(PIN_MOT_EN,HIGH);
 // digitalWrite(PIN_MOT_STP,LOW);
  digitalWrite(PIN_MOT_DIR,LOW);
 }


FinDeCourseSablier_e FdCMoteur(uint32_t gravityDeg)
{
  uint32_t delta = 10;

  if( (gravityDeg > 800) && (gravityDeg < 1000)   )
   return UPA ;
   
  if( (gravityDeg > 2600) && (gravityDeg < 2800)   )
   return UPB ;

   return undefined;
  
  
  
  }
 
/*

void stepperEnable()
{
  digitalWrite(PIN_MOT_EN,LOW);
}

void stepperDisable()
{
  digitalWrite(PIN_MOT_EN,HIGH);
}

void stepperHold()
{
 //#TODO Stopper la PWM et laisser les moteur actifs
  void stepperEnable();

 
}

void stepperTickCW()
{
  digitalWrite(PIN_MOT_STP,HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_MOT_STP,LOW);
  delayMicroseconds(1);
}
void stepperTickCCW()
{
  digitalWrite(PIN_MOT_STP,HIGH);
  delayMicroseconds(1);
  digitalWrite(PIN_MOT_STP,LOW);
  delayMicroseconds(1);
  
}

*/
