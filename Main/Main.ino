
/*-------------------Appel aux fichier filles----------*/

#ifndef CONSTANTDEFINE
  #include "ConstantDefine.h"
  #define CONSTANTDEFINE
#endif

#include "UserDefine.h"

#ifndef SANDGRAIN
  #include "SandGrain.h"
  #define SANDGRAIN
#endif

#include "Gravite.h"
#include "Affichage.h"
#include "Moteur.h"
#include "mbed.h"

#include "DHT.h"

/*-------------------Appel des bibliothèques-----------*/
#include <TimeLib.h>

#include <ArduinoBLE.h> //Ajout du Blutooth BLE intégré
#ifdef MULTILOOP
#include <Scheduler.h>
#endif
/*-------------------Machine a etat----------------*/
typedef enum
{
state_remplissage,
state_sablier,
state_sablier2,
state_heure,
state_temperature,
state_moteur,
state_test,
state_idling,
state_test2
 }machineState_e;

machineState_e machineState = state_remplissage;
/*-------------------Variables Globales-------------------*/
//Variable pour le temps
time_t temps = 0;

//Variables pour le remplissage
unsigned int decompteur_rempl = NBINITIAL;
unsigned int tickCounterRempl = 0;
//Variables pour le mode température
int temperature = 0 ;

//Variables pour la simulation de sable
Grain_t MatriceA[10][10];//Déclaration mémoire de deux matrices de 8x8 avec une bordure de mur, donc 10*10
Grain_t MatriceB[10][10];


unsigned long AngleGrav = 0 ; 
unsigned int decompteTransMatrice = TICKTRANSMATRICE; //Nombre de cycle entre chaque passage de grain

unsigned int compteur_test = 0;

//Variables Autres

uint8_t HourGrainIDList[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

unsigned long Compteur_millis_A = 0;
unsigned long Compteur_millis_B = 0;

bool sens = false;


typedef enum
{
motor_OFF = 0,
motor_AUP,
motor_BUP
 }positionSablier_e;

//positionSablier_e consigneMoteur
positionSablier_e memorientationCible = motor_OFF;
positionSablier_e orientationCible = motor_OFF;
FinDeCourseSablier_e FDCSablier;

BLEService ledService(LEDSERVICE_UUID); // BLE LED Service
BLEByteCharacteristic switchCharacteristic(SWITCHCHARACTERISTIC_UUID, BLERead | BLEWrite);
BLEUnsignedIntCharacteristic TimeSynchroCharacteristic(TIMESYNCHROCHARACTERISTIC_UUID,BLEWrite);
mbed::PwmOut pwm_1(digitalPinToPinName(PIN_MOT_STP));
DHT dht(PIN_DHT, DHTTYPE);

void serialInit()
{
  Serial.begin(9600);
  Serial.println("Hello World ! ;-) ");
}

void dht11Init()
{
  dht.begin();
}

void BLEInit()
{
  BLE.begin();
  
  BLE.setLocalName(NOMBLE);  // set the local name peripheral advertises
  
  BLE.setAdvertisedService(ledService); // set the UUID for the service this peripheral advertises
  
  ledService.addCharacteristic(switchCharacteristic);  // add the characteristic to the service
  ledService.addCharacteristic(TimeSynchroCharacteristic); // Ajout de la caractéritique d'écriture de synchro du temps au service
  
  BLE.addService(ledService);  // Ajout du service au BLE

  // assign event handlers for connected, disconnected to peripheral
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  // assign event handlers for characteristic
  switchCharacteristic.setEventHandler(BLEWritten, switchCharacteristicWritten);
  TimeSynchroCharacteristic.setEventHandler(BLEWritten , TimeSynchroCharacteristicWritten);
  
  // set an initial value for the characteristic
  switchCharacteristic.setValue(0);
  
  BLE.advertise();
}

#ifdef MULTILOOP
void schedulerInit()
{
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
}
#endif

/*-------------------------------------*/
//             Initialisations         //
//                                     //     
/*-------------------------------------*/

void setup() 
{
  
  serialInit();
  BLEInit();
  IMUInit();
  
  #ifdef MULTILOOP
  schedulerInit();
  #endif
  
  neopixelInit();
  stepperInit();
  dht11Init();
  
  initMatrice(MatriceA);
  initMatrice(MatriceB);
  setTime(23,59,0,26,9,2021);
  //temps = now();
}

/*-------------------------------------*/
//             Implémentation          //
//              des routines           //     
/*-------------------------------------*/
void loop() 
{
  //time_t temps = now();
  
  
   uint32_t* ptr_regstart = (uint32_t*)0x4000C000; 
  uint32_t* ptr_dtready = (uint32_t*)0x4000C100;
  uint32_t* ptr_regint = (uint32_t*)0x4000C304;
  uint32_t* ptr_value = (uint32_t*)0x4000C508;
  BLE.poll();

  //Serial.println(AngleGrav);
  
  switch(machineState)
  {
    //--------------------------------------------//
    //              Routine Remplissage           //
    //--------------------------------------------//
    case state_remplissage :
      delay(PERIODEREFRESH);

      AngleGrav = 450+1800;
     
      if(tickCounterRempl <= 0 )
      {
        addGrain(MatriceA,1,1); //Ajout d'un grain de sable
        MatriceA[1][1].grainId = decompteur_rempl;
        decompteur_rempl--;//Decrementation du compteur de remplissage
        tickCounterRempl = 5;
      }
      tickCounterRempl--;
      RunSimRules(MatriceA,AngleGrav);
      
      decayDeadColor(MatriceA);

      drawMatrice(MatriceA,MatriceB);
      break;  
    //--------------------------------------------//
    //              Routine Sablier               //
    //--------------------------------------------//
    case state_sablier : 
      
      AngleGrav = (2700+getGravityAngle()+450)%3600;
      
      
      
      
      
      if(decompteTransMatrice <= 0) // à la fin du décompte un grain passe d'une matrice à l'autre (et le decompte est réinitialisé)
      {
        
        if((AngleGrav > 1350) && (AngleGrav < 3150))
        {
          moveGrain2(MatriceA,MatriceB,8,8,1,1);
        }
        else
        {
          moveGrain2(MatriceB,MatriceA,1,1,8,8);
        }
      decompteTransMatrice = TICKTRANSMATRICE;
      }
      else // sinon on conitnue de décompter
      {
        decompteTransMatrice--;
      }
      RunSimRules(MatriceA,AngleGrav);
      RunSimRules(MatriceB,AngleGrav);
      
      temps = now();
      
      if(howMuchHourGrain(HourGrainIDList) < hour(temps)%12)
      {
      addRandHourGrain(HourGrainIDList);
      //Serial.println((sizeof(HourGrainIDList)/sizeof(HourGrainIDList[0])));
 
      }
      else if(howMuchHourGrain(HourGrainIDList) > hour(temps)%12)
      {
        clrRandHourGrain(MatriceA,MatriceB,HourGrainIDList);
      }
      
      //Serial.println(howMuchHourGrain(HourGrainIDList));
      colorizeHourGrain(MatriceA,MatriceB,HourGrainIDList);
      
      decayDeadColor(MatriceA);
      decayDeadColor(MatriceB);
      //decayDeadColor(MatriceA);
      drawMatrice(MatriceA,MatriceB);
      break;
    //--------------------------------------------//
    //              Routine Sablier 2             //
    //--------------------------------------------//
    case state_sablier2 : 
      
      AngleGrav = (1800+getGravityAngle())%3600;
      
      
      
      RunSimRules(MatriceA,AngleGrav);
      RunSimRules(MatriceB,AngleGrav);
      
      if(decompteTransMatrice <= 0) // à la fin du décompte un grain passe d'une matrice à l'autre (et le decompte est réinitialisé)
      {
        if(AngleGrav > 1800)//#TODO seuil à ajuster
        {
          moveGrain2(MatriceA,MatriceB,8,8,1,1);
        }
        else
        {
          moveGrain2(MatriceB,MatriceA,1,1,8,8);
        }
      decompteTransMatrice = TICKTRANSMATRICE;
      }
      else // sinon on conitnue de décompter
      {
        decompteTransMatrice--;
      }

      decayDeadColor(MatriceA);
      decayDeadColor(MatriceB);
      //decayDeadColor(MatriceA);
      drawMatrice(MatriceA,MatriceB);
      break;
  
    //--------------------------------------------//
    //              Routine Température           //
    //--------------------------------------------//
    case state_temperature : 
     
      temperature = dht.readTemperature();
      delay(2000);
      Serial.println(temperature);
      drawDigit(temperature/10,temperature%10,0,0);
      
      break;
    //--------------------------------------------//
    //              Routine Horloge               //
    //--------------------------------------------//
     case state_heure : 
     temps = now();

     /*
     Serial.println("---------------------") ;
     Serial.print("Sync: ");
     Serial.println(timeStatus()) ;
     
     Serial.print("Heure(s) : ");
     Serial.println(hour(temps) ) ;
     Serial.print("Minute(s) : ");
     Serial.println(minute(temps) ) ;
     Serial.print("Seconde(s) : ");
     Serial.println(second(temps) ) ;
   */
      drawDigit(ceil(hour(temps)/10),hour(temps)%10,ceil(minute(temps)/10),minute(temps)%10);
      delay(100);
      break;
  
    //--------------------------------------------//
    //              Routine MOTEUR                //
    //--------------------------------------------//
     case state_moteur: 

      AngleGrav = (1800+getGravityAngle())%3600;
      Serial.println(AngleGrav);

      FDCSablier = FdCMoteur(AngleGrav);

      if((FDCSablier == UPA) && (orientationCible == motor_AUP) )
          orientationCible = motor_OFF;

      if((FDCSablier == UPB) && (orientationCible == motor_BUP) )
          orientationCible = motor_OFF;
      
      if(( howManyGrain(MatriceA) <= 0 )) // Condition de mise en route
         orientationCible = motor_BUP;

      if(( howManyGrain(MatriceB) <= 0 )) // Condition de mise en route 
         orientationCible = motor_AUP;

      if((orientationCible != memorientationCible)|| true)//wip
      {
        switch(orientationCible)
        {  
         case motor_AUP:
          pwm_1.write(0.5);
          pwm_1.period_us(3000);
          digitalWrite(PIN_MOT_EN,LOW);
         break;
         case motor_BUP:
          pwm_1.write(0.5);
          pwm_1.period_us(3000);
          digitalWrite(PIN_MOT_EN,LOW);
         break;
         case motor_OFF:
         
          digitalWrite(PIN_MOT_EN,HIGH);
         break;
        }
      memorientationCible = orientationCible;
      }
      
      

      
    
      /*  
      digitalWrite(PIN_MOT_EN,LOW);
      digitalWrite(PIN_MOT_DIR,sens);
      //analogWrite(PIN_MOT_STP,125);
      //pwm_1.period_us(100);
      //pwm_1.pulsewidth_us(50);
      pwm_1.write(0.5);
      
      for(int i = 20000; i >= 1000 ;i = i - 50 )
      {
        pwm_1.period_us(i);
        delay(100);
        Serial.print(i);
        Serial.println("fast");
      }
      
      //pwm_1.pulsewidth_us(0);
      delay(2000);
      
      
      //pwm_1.pulsewidth_us(50);
      
      
      for(int i = 1000; i <= 20000 ;i = i + 50 )
      {
        pwm_1.period_us(i);
        delay(100);
        Serial.print(i);
        Serial.println("slow");
      }
     pwm_1.write(0);
      delay(2000);
      digitalWrite(PIN_MOT_EN,HIGH);
      sens = !sens;
    */
      
      break;
    
      
    //--------------------------------------------//
    //              Routine Test                  //
    //--------------------------------------------//
     case state_test2: 
     

      
     break;
     case state_idling:
      //delay(100);
     

     break;
      
  }
// Fin des routines

//gestion des transitions
  
   if((decompteur_rempl <=0) && (machineState == state_remplissage) )  /// TRANSITION POST_INIT
  {
    machineState = state_idling;    
  }

  if(machineState == state_sablier) // Transion de fin de mode sablier
  {
    machineState = state_idling ; 
    Compteur_millis_A = millis();
  }
  if(((millis()-Compteur_millis_A) > PERIODEREFRESH )   && (machineState == state_idling )  ) // Transition de mise en mode sablier
  {
    
    machineState = state_sablier; 
  }

  
  if(machineState == state_moteur) // Transion de fin de mode sablier
  {
    machineState = state_idling ; 
    Compteur_millis_B = millis();
  }

  if(((millis()-Compteur_millis_B) > PERIODEMOTEUR )   && (machineState == state_idling ) && ENABLE_AUTOROTATE ) // Transition de mise en mode moteur
  {
    
    machineState = state_moteur; 
  }

  
  
   


//FIN DES TRANSITION 
  
}
#ifdef MULTILOOP
void loop2()
{

}

void loop3()
{

  //digitalWrite(PIN_MOT_EN,LOW);
  //analogWrite(PIN_MOT_STP,125);
  //pwm_1.period_ms(100);
  //pwm_1.write(0.5);
  //digitalWrite(PIN_MOT_STP,HIGH);
  //delayMicroseconds(1);
  //digitalWrite(PIN_MOT_STP,LOW);
  //delayMicroseconds(1);
  yield();
}

#endif

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void switchCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print("Characteristic event, written: ");
  drawDigit(switchCharacteristic.value(),0,0,0);
   
}

void TimeSynchroCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic)
{
  Serial.println(TimeSynchroCharacteristic.value());
  Serial.println(temps);
  setTime(TimeSynchroCharacteristic.value()+7200);
}
