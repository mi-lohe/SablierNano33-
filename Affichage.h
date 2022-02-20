
#ifndef CONSTANTDEFINE
#include "ConstantDefine.h"
#define CONSTANTDEFINE
#endif

#ifndef SANDGRAIN
#include "SandGrain.h"
#define SANDGRAIN
#endif

#include <Adafruit_NeoPixel.h> //Pour les matrice de LED WS2812.


void neopixelInit();

void drawMatrice(Grain_t [10][10],Grain_t [10][10]);
void drawDigit(unsigned int ,unsigned int ,unsigned int , unsigned int);
void powerStressTest();
