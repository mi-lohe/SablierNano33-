
#include "Affichage.h"

Adafruit_NeoPixel MatricesLED(LED_QTY,PIN_MATRICES_LED,NEO_GRB + NEO_KHZ800); //Object MatriceLED pour les NEOPIXEL

const byte arrayN[10][3] =
{
  {
    //Numéro 0
    0b00111100,
    0b01000010,
    0b00111100
  },
  {
    //Numéro 1
    0b00001000,
    0b00000100,
    0b01111110
  },
  {
    //Numéro 2
    0b01100100,
    0b01010010,
    0b01001100
  },
  {
    //Numéro 3
    0b01001010,
    0b01001010,
    0b00111100
  },
  {
    //Numéro 4
    0b00001110,
    0b00001000,
    0b01111110
  },
  {
    //Numéro 5
    0b01000110,
    0b01001010,
    0b00110010
  },
  {
    //Numéro 6
    0b00111100,
    0b01001010,
    0b00110010
  },
  {
    //Numéro 7
    0b00000110,
    0b00010010,
    0b01111110
  },
  {
    //Numéro 8
    0b00110100,
    0b01001010,
    0b00110100
  },
  { 
    //Numéro 9
    0b00000100,
    0b01001010,
    0b00111100
  }
};



void neopixelInit()
{

MatricesLED.begin();

MatricesLED.show();

 }


void drawMatrice(Grain_t MA[10][10],Grain_t MB[10][10])
{
  //Récupère la couleur de chaque grain et l'affiche à position qui y correpond
  uint8_t i;
  uint8_t j;
  uint8_t index = 0;

  MatricesLED.clear();

  for(i=1;i<9;i++)
  {
    for(j = 1 ; j<9 ; j++)
    {
      MatricesLED.setPixelColor(index, MA[i][j].color); //  Set pixel's color (in RAM)
      index++;
    }
  }

  for(i = 1;i<9;i++)
  {
      for(j = 1 ; j < 9 ; j++)
      {
        //LED_MATRIX_send_pixel(MB[i][j].color);
        MatricesLED.setPixelColor(index, MB[i][j].color); //  Set pixel's color (in RAM)
        index++;
      }
  }
  //MatricesLED.clear();
  MatricesLED.show();
}

void drawDigit(unsigned int a1,unsigned int a2,unsigned int b1, unsigned int b2)
{
  MatricesLED.clear();

  //Chargement sur les dizaine de la première matrice
  for(int i = 0 ; i <= 2 ; i++)
  {
    for(int j = 0; j <= 7 ; j++)
    {
      if(  arrayN[a1][i] & (1<<j)  )
      {
        MatricesLED.setPixelColor( (j*8) + (i+1),0xB55400);//Rotation de 90° en inversant i et j
      }
    }  
  }
  //Chargement sur les unités de la première matrice
  for(int i = 0 ; i <= 2 ; i++)
  {
    for(int j = 0; j <= 7 ; j++)
    {
      if(  arrayN[a2][i] & (1<<j)  )
      {
        MatricesLED.setPixelColor( (j*8) + (i+4),0xEB5E0B);//Rotation de 90° en inversant i et j
      }
    }  
  }
    
  MatricesLED.show();
}

void powerStressTest()
{
  for(int i = 0;i <= 128;i++)
  MatricesLED.setPixelColor(i,0xFFFFFF);
  MatricesLED.show();
}
