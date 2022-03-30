
#ifndef CONSTANTDEFINE
#include "ConstantDefine.h"
#define CONSTANTDEFINE
#endif

#include "Arduino.h"

typedef enum 
{
  undefined,
  lateralDr,
  lateralGa,
  UPA,
  UPB
  }FinDeCourseSablier_e;
  
void stepperInit();
FinDeCourseSablier_e FdCMoteur(uint32_t);
