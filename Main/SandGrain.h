
#include "Arduino.h"

typedef struct  //structure du type grain
{
  uint8_t grainId;
  bool exist;
  bool isUpdated;
  uint32_t color;
}Grain_t;


//Gestion Primaire
Grain_t angNeighbor(Grain_t [10][10],uint8_t,uint8_t,uint32_t);
uint32_t colorRGB(uint32_t,uint32_t,uint32_t);

void angCoord(uint8_t ,uint8_t ,uint32_t ,uint8_t* ,uint8_t* );

void addGrain(Grain_t [10][10],uint8_t,uint8_t);

void suprGrain(Grain_t [10][10],uint8_t ,uint8_t);

//Gestion de Second niveau

void moveGrain(Grain_t [10][10],uint8_t ,uint8_t ,uint8_t ,uint8_t) ;

void moveGrain2(Grain_t [10][10],Grain_t [10][10],uint8_t ,uint8_t ,uint8_t ,uint8_t );

//Gestion de Troisième niveau

void initMatrice(Grain_t [10][10]);

void RunSimRules(Grain_t [10][10],uint32_t);

void decayDeadColor(Grain_t [10][10]);

bool isMatrixEmpty(Grain_t [10][10]);

int howManyGrain(Grain_t [10][10]);

int howManyGrain(Grain_t [10][10],uint32_t);

//Gestion des Pixel Heures

bool locateID(Grain_t [10][10],Grain_t [10][10],uint8_t ,bool* ,uint8_t* ,uint8_t* );

uint8_t howMuchHourGrain(uint8_t [24]);

void addRandHourGrain(uint8_t [24]);

void colorizeHourGrain(Grain_t [10][10],Grain_t [10][10],uint8_t [24]);

void clrRandHourGrain(Grain_t [10][10],Grain_t [10][10],uint8_t [24]);
