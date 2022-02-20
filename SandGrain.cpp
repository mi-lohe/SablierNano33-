#ifndef CONSTANTDEFINE
#include "ConstantDefine.h"
#endif


#include "SandGrain.h"
#include "UserDefine.h"



Grain_t angNeighbor(Grain_t Minput[10][10],uint8_t x,uint8_t y,uint32_t a)
{
  //récupère le Grain qui est au voisinage direct à un angle donné
  //( ex :si a = 0, on  a accèes au grain du dessus,avec a = 2700 on a celui de gauche )

  //a = MODULO(a+225,3600);

  a = (a+225) % 3600;
  
  if(a < 450){
    return Minput[x][y+1];
  }
  else if((a >= 450) && (a < 900)){
    return Minput[x+1][y+1];
  }
  else if((a >= 900) && (a < 1350)){
    return Minput[x+1][y];
  }
  else if((a >= 1350) && (a < 1800)){
    return Minput[x+1][y-1];
  }
  else if((a >= 1800) && (a < 2250)){
    return Minput[x][y-1];
  }
  else if((a >= 2250) && (a < 2700)){
    return Minput[x-1][y-1];
  }
  else if((a >= 2700) && (a < 3150)){
    return Minput[x-1][y];
  }
  else if((a >= 3150) && (a < 3600)){
    return Minput[x-1][y+1];
  }

  return Minput[0][0];
}


uint32_t colorRGB(uint32_t R,uint32_t G,uint32_t B)
{
  // Transforme trois canaux RGB en Valeur compréhensible par le chip WS2812
  return(G<<16) + (R<<8) + B;
}

void angCoord(uint8_t in_x,uint8_t in_y,uint32_t a,uint8_t* out_x,uint8_t* out_y)
{
  {

    //Calcule la coordonnée directement voisine en fonction de l'angle spécifié
    //a = MODULO(a+225,3600);
    a = (a+225) % 3600;
    
    if(a < 450){
      *out_x = in_x;
      *out_y = in_y + 1;
      return;
    }
    else if((a >= 450) && (a < 900)){
      *out_x = in_x + 1;
      *out_y = in_y + 1;
      return;
    }
    else if((a >= 900) && (a < 1350)){
      *out_x = in_x + 1;
      *out_y = in_y;
      return;
    }
    else if((a >= 1350) && (a < 1800 )){
      *out_x = in_x + 1;
      *out_y = in_y - 1;
      return;
    }
    else if((a >= 1800) && (a < 2250)){
      *out_x = in_x;
      *out_y = in_y - 1;
      return;
    }
    else if((a >= 2250) && (a < 2700)){
      *out_x = in_x - 1;
      *out_y = in_y - 1;
      return;
    }
    else if((a >= 2700) && (a < 3150)){
      *out_x = in_x - 1;
      *out_y = in_y;
      return;
    }
    else if((a >= 3150) && (a < 3600)){
      *out_x = in_x - 1;
      *out_y = in_y + 1;
      return;
      }
  }

}
// IDEA ajouter des variation random à la couleur du sable
void addGrain(Grain_t M[10][10],uint8_t x,uint8_t y)
{
  //Cette fonction crée un grain couleur sable en le déclaarant comme non traité
  M[x][y].color = GRAINCOLOR ;
  M[x][y].isUpdated = false;
  M[x][y].exist = true;

}

void suprGrain(Grain_t M[10][10],uint8_t x,uint8_t y)
{
  //Cette fonction remet a zero les paramètres d'un grains
  M[x][y].grainId = 0;
  M[x][y].color = 0x000700;
  M[x][y].isUpdated = true;
  M[x][y].exist = false;

}

void moveGrain(Grain_t M[10][10],uint8_t fx,uint8_t fy,uint8_t tx,uint8_t ty)
{
  //Cette fonction copie un grain source dans un grain cible et supprime la source
  if(M[tx][ty].exist == false) // Vérif pour eviter un écrasement de donnée
  {
  M[tx][ty]= M[fx][fy];
  suprGrain(M,fx,fy);
  }
}

void moveGrain2(Grain_t mA[10][10],Grain_t mB[10][10],uint8_t fx,uint8_t fy,uint8_t tx,uint8_t ty)
{
  if((mB[tx][ty].exist == false) && (mA[fx][fy].exist == true) ) // Vérif pour eviter un écrasement de donnée
  {
  mB[tx][ty]= mA[fx][fy];
  suprGrain(mA,fx,fy);
  }
}

void initMatrice(Grain_t M[10][10])
{
  //Met a zero l'ensemble des grains d'une matrice
  //crée au passage les bordures de limitation, devant empêcher la fuite des grains.
  uint8_t i;
  uint8_t j;

  for(i = 0;i<9;i++){
    for(j = 0;j<9;j++)
    {
  M[i][j].color = 0x000000;
  M[i][j].isUpdated = false;
  M[i][j].exist = false;
    }
  }

  for(i = 0;i<9;i++){
    M[0][i].exist = true;
  }
  for(i = 0;i<9;i++){
    M[9][i].exist = true;
  }
  for(i = 0;i<=9;i++){
    M[i][0].exist = true;
  }
  for(i = 0;i<=9;i++){
    M[i][9].exist = true;
  }

}


void RunSimRules(Grain_t MA[10][10],uint32_t gravityDeg)
{
  //Fonction chargée d'appliquer les règles de l'automate cellulaire sur la matrice passée en pramètre

  uint8_t xtemp,ytemp;

  //Remise à zero des flag de mise a jour
  for(uint8_t x = 1 ; x < 9; x++)
  for(uint8_t y = 1 ; y < 9 ; y++)
    {
      MA[x][y].isUpdated = false;
    }

  for(uint8_t x = 1 ; x < 9; x++ )  //on balaye la matrice en quête de grain à traiter ...
    for(uint8_t y = 1 ; y < 9 ; y++)
    {
      if((MA[x][y].exist == true) && (MA[x][y].isUpdated == false ))
      {

        if( angNeighbor(MA,x,y,gravityDeg +1800).exist == false ) //Si il n'y a rien sous le grain , il tombe
        {
          angCoord(x,y, gravityDeg +1800,&xtemp,&ytemp);
          moveGrain(MA,x,y,xtemp,ytemp);
        }
        else if( angNeighbor(MA,x,y,gravityDeg + 2250).exist == false )
        {
          angCoord(x,y,gravityDeg + 2250,&xtemp,&ytemp);
          moveGrain(MA,x,y,xtemp,ytemp);

        }
        else if( angNeighbor(MA,x,y,gravityDeg + 1350).exist == false )
        {
          angCoord(x,y,gravityDeg + 1350,&xtemp,&ytemp);
          moveGrain(MA,x,y,xtemp,ytemp);
        }
        MA[xtemp][ytemp].isUpdated = true;
      }


    }

}

void decayDeadColor(Grain_t M[10][10]) //Decremente les trois canaux couleur d'un pixel "mort"
{
  uint32_t color;
  uint32_t R,G,B;

  for(int x = 1; x < 9;x++)
  {
    for(int y = 1 ; y < 9; y++)
    {
      if(M[x][y].exist == false)
      {
        color = M[x][y].color;
        B = color & 0x0000FF;
        R = (color & 0x00FF00) >> 8;
        G = (color & 0xFF0000) >> 16;
        //GRB ,où B est MSB
        if(B > 0)
        {
          M[x][y].color = colorRGB(R,G,B-1);
        }
        if(R > 0)
        {
          M[x][y].color = colorRGB(R-1,G,B);
        }
        if(G > 0)
        {
          M[x][y].color = colorRGB(R,G-1,B);
        }
      }
    }
  }
}

int howManyGrain(Grain_t M[10][10])
{
  int grainCounter = 0;

  for(int x = 1; x < 9;x++)
  {
    for(int y = 1; y < 9;y++)
    {
      if(M[x][y].exist == true)
      {
        grainCounter++;
      }
    }
  }
  return grainCounter;
  }
int howManyGrain(Grain_t M[10][10],uint32_t color)
{
  int grainCounter = 0;

  for(int x = 1; x < 9;x++)
  {
    for(int y = 1; y < 9;y++)
    {
      if(M[x][y].exist == true)
      {
        grainCounter++;
      }
    }
  }
  return grainCounter;
 }

bool isMatrixEmpty (Grain_t M[10][10])
{
  bool emptyFlag = true;

  for(int x = 1 ; x < 9 ; x++) 
  {
    for(int y = 1 ; y < 9 ; y++)
    {
      if(M[x][y].exist == true)
      {
      emptyFlag == false;
      break;
      } 
    }
    if(emptyFlag == true)
    break;  
  }
  return emptyFlag;
}


//#IDEA : UTLISER LE REGITRE DU RNG Termqiue 


bool RunSimRules2(Grain_t MA[10][10],uint32_t gravityDeg)//Comme la version 1 mais ajoute de la randomisation et d
{
  //Fonction chargée d'appliquer les règles de l'automate cellulaire sur la matrice passée en pramètre
  bool motionFlag = false;
  uint8_t xtemp,ytemp;

  //Remise à zero des flag de mise a jour
  for(uint8_t x = 1 ; x < 9; x++)
  for(uint8_t y = 1 ; y < 9 ; y++)
    {
      MA[x][y].isUpdated = false;
    }


  for(uint8_t x = 1 ; x < 9; x++ )  //on balaye la matrice en quête de grain à traiter ...
  for(uint8_t y = 1 ; y < 9 ; y++)
  {
    if((MA[x][y].exist == false) && (MA[x][y].isUpdated == false ))
    {

      if( angNeighbor(MA,x,y,gravityDeg +1800).exist == false ) //Si il n'y a rien sous le grain , il tombe... ce mouvement est toujour prioritaire
      {
        angCoord(x,y, gravityDeg +1800,&xtemp,&ytemp);
        moveGrain(MA,x,y,xtemp,ytemp);
        motionFlag = true;
      }
      else //Si la place est prise on regarde sur les bords
      {
        switch(rand()%1) //on prend au hasard un scenario où la priorté est soit a gauche soit a droite
        {
          case 0 :
            if( angNeighbor(MA,x,y,gravityDeg + 2250).exist == false )
            {
              angCoord(x,y,gravityDeg + 2250,&xtemp,&ytemp);
              moveGrain(MA,x,y,xtemp,ytemp);

            }
            else if( angNeighbor(MA,x,y,gravityDeg + 1350).exist == false )
            {
              angCoord(x,y,gravityDeg + 1350,&xtemp,&ytemp);
              moveGrain(MA,x,y,xtemp,ytemp);
            }
          break;
          case 1 :
            if( angNeighbor(MA,x,y,gravityDeg + 1350).exist == false )
            {
              angCoord(x,y,gravityDeg + 1350,&xtemp,&ytemp);
              moveGrain(MA,x,y,xtemp,ytemp);
            }
            else if( angNeighbor(MA,x,y,gravityDeg + 2250).exist == false )
            {
              angCoord(x,y,gravityDeg + 2250,&xtemp,&ytemp);
              moveGrain(MA,x,y,xtemp,ytemp);
            }
          break;
        }
      MA[xtemp][ytemp].isUpdated = true;
      }
    }
  }
return motionFlag;
}


bool locateID(Grain_t MA[10][10],Grain_t MB[10][10],uint8_t wantedID,bool* A_trueB_false,uint8_t* foundX,uint8_t* foundY)
{
  
  for(uint8_t x = 1 ; x < 9; x++ )  //on balaye la matrice en quête de grain à traiter ...
  for(uint8_t y = 1 ; y < 9; y++ )
  {

    if(MA[x][y].grainId == wantedID )
    {
      *foundX = x;
      *foundY = y;
      *A_trueB_false = true;
      return true;
    }
  }
  
  for(uint8_t x = 1 ; x < 9; x++ ) 
  for(uint8_t y = 1 ; y < 9; y++ )  //on cherche dans l'autre matrice si on a pas trouvé et A_highB_low passe à false
  {
    if(MB[x][y].grainId == wantedID )
    {
      *foundX = x;
      *foundY = y;
      *A_trueB_false = false;
      return true;
    }
  }
  return false;
}


uint8_t howMuchHourGrain(uint8_t CheckList[24])
{
  int qty = 0;

  for(uint8_t i = 0; i < 24;i++)
    {
      if(CheckList[i] > 0)
      {
      qty ++;
      }
      else
      {
        break;
      }
    }
  
  return qty;
}

void addRandHourGrain(uint8_t CheckList[24])
{
  uint8_t NbHourGrain;
  int newComer = 0 ; 
  NbHourGrain = howMuchHourGrain(CheckList);
    
  //howManyHourGrain renvoie le nombre d'HourGrain
  
  // Choix d'un ID au hasard et qui n'est pas déja dans le tableau 
  bool validID = true;
  do 
  {
    
    newComer = random(1, NBINITIAL);
    validID = true;
    for(uint8_t y = 0; y < NbHourGrain-1 ; y++) //Si un doublon est détecté on retire 
    {
      
      if(CheckList[y] == newComer )
       validID = false;
    }
  }while(!validID);

// Ajout de l'ID tiré à la liste 
  if(NbHourGrain < 23)
  {
  CheckList[NbHourGrain] = newComer;
  }
}

void colorizeHourGrain(Grain_t MA[10][10],Grain_t MB[10][10],uint8_t CheckList[24])
{
  uint8_t posX = 0;
  uint8_t posY = 0;
  bool MatAB = false;

  for(uint8_t index = 0; index < howMuchHourGrain(CheckList) ; index++)
  {
    if(locateID(MA,MB,CheckList[index],&MatAB,&posX,&posY))
    {
      if(MatAB) //A_true B_false
      {
        MA[posX][posY].color = HOURGRAINCOLOR;
      }
      else
      {
        MB[posX][posY].color = HOURGRAINCOLOR; 
      }
    }
    
  }
  
}

void clrRandHourGrain(Grain_t MA[10][10],Grain_t MB[10][10],uint8_t CheckList[24])
{
//On redonne une couleur normale à tout les Hourgrain

  
for(uint8_t i = 0; i < (sizeof(CheckList)/sizeof(CheckList[0]));i++)
CheckList[i] = 0;
  
}
