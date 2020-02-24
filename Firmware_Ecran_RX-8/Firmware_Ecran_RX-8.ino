#include <EEPROM.h>

// Address en EEPROM des variable
#define adr_tilt  0 
#define adr_ouvert  10
#define adr_pos_close  20
#define adr_pos_max  30
#define adr_pos_min  40
#define adr_initialisation  50

// Addressage des pin
#define POS_SIGNAL_PIN 1  // Signal de position de l'écran
#define SIG_APC 8         // Entrer numérique de + apc
#define TILT_BT 7         // Bouton Tilt
#define OPEN_BT 5         // Bouton Open
#define RELAY_ALIM_APC 4  //coupure du +APC
#define RELAY_ALIM_PP 6   // Relais + permanant
#define RELAY_AUX_LCD 9   // Relais LCD
#define RELAY_AUX_PI 10   // Contact Raspberry Pi
#define RIN_PIN A3        // Pin de commande moteur
#define FIN_PIN 3         // Pin de commande moteur

//Variable avec valeur par défaut "bidon"
int consigne = 400;
byte ouvert = 0;
byte tilt = 0;
int pos_close = 30;
int pos_min = 400;
int pos_max = 730;
int hyst = 15;
int intervale = (pos_max-pos_min)/3;
byte regulation=0;
byte initialisation = 0;

#define TIMER 30000 // en ms

void setup() {
//  Serial.begin(9600);
  pinMode(SIG_APC,INPUT);
  pinMode(POS_SIGNAL_PIN,INPUT);
  
  pinMode(OPEN_BT,INPUT);
  pinMode(TILT_BT,INPUT);
  
  pinMode(RELAY_ALIM_APC,OUTPUT);
  pinMode(RELAY_ALIM_PP,OUTPUT);

  pinMode(RELAY_AUX_LCD,OUTPUT);
  pinMode(RELAY_AUX_PI,OUTPUT);

  pinMode(FIN_PIN,OUTPUT);
  pinMode(RIN_PIN,OUTPUT);
  digitalWrite(RELAY_AUX_PI,0);
  digitalWrite(RELAY_ALIM_PP,1);
  delay(500);
  digitalWrite(RELAY_ALIM_APC,1);
  digitalWrite(RELAY_AUX_LCD,0);

//Récupération des valeur enregistré en mémoire
   tilt = EEPROM.read(adr_tilt);
   ouvert = EEPROM.read(adr_ouvert);
// procédure de controle que tilt et open sont OK sinon on ecrit les valeurs par défaut
   if (ouvert!=1 and ouvert!=0){
    ouvert=0;
    EEPROM.write(adr_ouvert,ouvert);
   }
   if (tilt!=0 and tilt!=1 and tilt !=2 and tilt != 3){
    tilt=0;
    EEPROM.write(adr_tilt,tilt);
   }
   
// controle si le système a déjà été initialisé
  if (EEPROM.read(adr_initialisation)==1){
    initialisation = 1;
    regulation = 1;
    pos_close = EEPROM.read(adr_pos_close)*4+EEPROM.read(adr_pos_close+1);
    pos_min = EEPROM.read(adr_pos_min)*4+EEPROM.read(adr_pos_min+1);
    pos_max = EEPROM.read(adr_pos_max)*4+EEPROM.read(adr_pos_max+1);
    intervale = (pos_max-pos_min)/3;
    consigne=pos_max-tilt*intervale;
  }else{
// Défini que le système n'est pas parametrer et va mettre le potentiommetre en position 200 sur 1024
    initialisation = 0;
    regulation = 0;
    consigne = 200;
    if(analogRead(POS_SIGNAL_PIN)>200){
    while (analogRead(POS_SIGNAL_PIN)>200){
      mot_fermeture();
    }
      mot_stop();
    }else{
      while (analogRead(POS_SIGNAL_PIN)<200){
      mot_ouverture();
    }
      mot_stop();
    }
  }
  delay(5000);
  Initialisation_Pos();  
  //delay(10000);
}

/*
* Boucle principal
*/ 
void loop(){
  // Si +après contact présent boucle principale
  if(!digitalRead(SIG_APC)==1){
    //Serial.println("Régulation de position");
    Regulation_pos();
  }else{
    // Si coupure du + après contact procédure de coupure de courant
      //Serial.println("Coupure Alimentation"); 
      coupure_Alim();
  }
}

/*
 * Boucle de régulation de la position
 */
void Regulation_pos(){
     // si l'écran est ouvert
     //Serial.print("Etat de l'écran=");
     //Serial.println(ouvert);
     //Serial.print("Position Ecran");
     //Serial.println(analogRead(POS_SIGNAL_PIN));
           
      if (ouvert == 1){
        digitalWrite(RELAY_AUX_LCD,1);
        consigne=pos_max-tilt*intervale;
        //Serial.print("Consigne=");
        //Serial.println(consigne);
        if (analogRead(POS_SIGNAL_PIN)>consigne+hyst and regulation==1){
          mot_stop();
          mot_fermeture();
          //Serial.println("Fermenture");
        }else if (analogRead(POS_SIGNAL_PIN)<consigne-hyst and regulation == 1){
          mot_stop();
          mot_ouverture();
          //Serial.println("Ouverture");
        }else{
          mot_stop();
          regulation = 0;
        }
        // Détection de l'appuis sur le bouton open/close
        if (!digitalRead(OPEN_BT)==1){
          ouvert=0;
          regulation = 1;
          while(!digitalRead(OPEN_BT)==1){
            delay(100); //Attendre le relachement du bouton
          }
        }
       // Détection d'un appui sur tilt 
        if (!digitalRead(TILT_BT)==1){
          tilt=tilt+1;
          if (pos_max-tilt*intervale < pos_min-hyst){
            tilt=0;
          }
          consigne=pos_max-tilt*intervale;
          regulation = 1;
          // Attente relachement du bouton
          while(!digitalRead(TILT_BT)==1){
            delay(100);
          }  
        }
      }else{
          digitalWrite(RELAY_AUX_LCD,0);
      // Si en position fermer
        if (analogRead(POS_SIGNAL_PIN)>pos_close+hyst and regulation == 1){
            mot_stop();
            mot_fermeture();
          }else{
            mot_stop();
            regulation = 0;
          }
          // Détection de l'appuis sur le bouton open/close
        if (!digitalRead(OPEN_BT)==1){
          ouvert=1;
          regulation = 1;
          while(!digitalRead(OPEN_BT)==1){
            delay(100); //Attendre le relachement du bouton
          }
        }
      }  
}

/*
 * Fonction d'apprentisage des positions
 */
void Initialisation_Pos(){
  //Serial.print("Bouton Open=");
  //Serial.println(!digitalRead(OPEN_BT));
  //Serial.print("Bouton Tilt=");
  //Serial.println(!digitalRead(TILT_BT));
  if (!digitalRead(OPEN_BT)==1 and !digitalRead(TILT_BT)==1){
      delay(1500);
      if (!digitalRead(OPEN_BT)==1 and !digitalRead(TILT_BT)==1){
        delay(1500);
        if (!digitalRead(OPEN_BT)==1 and !digitalRead(TILT_BT)==1){
          delay(1500);
          if (!digitalRead(OPEN_BT)==1 and !digitalRead(TILT_BT)==1){
            delay(1500);
              if (!digitalRead(OPEN_BT)==1 and !digitalRead(TILT_BT)==1){
                  // phase apprenttisage
                mot_ouverture();
                delay(500);
                mot_fermeture();
                delay(500);
                mot_ouverture();
                delay(500);
                mot_fermeture();
                mot_stop();
                while(!digitalRead(OPEN_BT)==1 or !digitalRead(TILT_BT)==1){
                  delay(100);
                }
                //Serial.println("Attente Position Fermer");
                while(!digitalRead(OPEN_BT)==0){
                  //Serial.println(analogRead(POS_SIGNAL_PIN));
                  delay(100);
                }
                pos_close = analogRead(POS_SIGNAL_PIN);
                //Serial.print("Position Fermer=");
                //Serial.println(pos_close);
                while(!digitalRead(OPEN_BT)==1){
                   delay(100);
                }
                //Serial.println("Attente Position Ouverte maxi");
                while(!digitalRead(OPEN_BT)==0){
                   delay(100);
                }
                
                pos_max = analogRead(POS_SIGNAL_PIN);
                //Serial.print(" Position Ouverte maxi=");
                //Serial.println(pos_max);
                while(!digitalRead(OPEN_BT)==1){
                   delay(100);
                }
                //Serial.println("Attente Position Ouverte mini");
                while(!digitalRead(OPEN_BT)==0){
                   delay(100);
                }
                pos_min = analogRead(POS_SIGNAL_PIN);
                //Serial.print("Position Ouverte mini=");
                //Serial.println(pos_min);

                intervale = (pos_max-pos_min)/3;

                if (EEPROM.read(adr_initialisation) != 1){
                  EEPROM.write(adr_initialisation,1);
                  //Serial.println("Ecritiure initialisation OK");
                }      
                if ((EEPROM.read(adr_pos_close)*4+EEPROM.read(adr_pos_close+1)) != pos_close){
                  EEPROM.write(adr_pos_close,pos_close >> 2); 
                  EEPROM.write(adr_pos_close+1,pos_close &0x03);
                  //Serial.println("Ecritiure position fermer OK");
                  //Serial.println(EEPROM.read(adr_pos_close)*4+EEPROM.read(adr_pos_close+1));
                }      
                if (EEPROM.read(adr_pos_max) != pos_max/4){
                  EEPROM.write(adr_pos_max,pos_max >> 2);
                  EEPROM.write(adr_pos_max+1,pos_max &0x03);
                  //Serial.println("Ecritiure position ouverte maxi OK");
                  //Serial.println(EEPROM.read(adr_pos_max)*4+EEPROM.read(adr_pos_max+1));
                }   
                if (EEPROM.read(adr_pos_min) != pos_min/4){
                  EEPROM.write(adr_pos_min, pos_min >> 2);
                  EEPROM.write(adr_pos_min+1, pos_min &0x03);
                  //Serial.println("Ecritiure position ouverte mini OK");
                  //Serial.println(EEPROM.read(adr_pos_min)*4+EEPROM.read(adr_pos_min+1));
                }    
              }
            }
          }
        }
      }  
}

/*
 * Fonction en cas de coupure du + après contact
 */
void coupure_Alim(){
  digitalWrite(RELAY_AUX_LCD,0);
 if(ouvert != EEPROM.read(adr_ouvert)){
    EEPROM.write(adr_ouvert,ouvert);
 }
 if(tilt != EEPROM.read(adr_tilt)){
    EEPROM.write(adr_tilt,tilt);
 }
 
 if (analogRead(POS_SIGNAL_PIN)>pos_close+hyst){
      mot_fermeture();
      regulation=0;
      while(analogRead(POS_SIGNAL_PIN)>pos_close+hyst){
        delay(10);
      }
      delay(100);
  }else{
    regulation = 0;
  }
  mot_stop();
  delay(TIMER);
  digitalWrite(RELAY_ALIM_PP,0);
}

void mot_fermeture(){
    digitalWrite(FIN_PIN,1);
    analogWrite(RIN_PIN,0);
   // digitalWrite(RIN_PIN,1);
}

void mot_ouverture(){
  analogWrite(RIN_PIN,255);
  //digitalWrite(RIN_PIN,0);
  digitalWrite(FIN_PIN,0);
}

void mot_stop(){
  digitalWrite(FIN_PIN,0);
  //digitalWrite(RIN_PIN,0);
  analogWrite(RIN_PIN,0);
}

void mot_break(){
  digitalWrite(FIN_PIN,0);
  //digitalWrite(RIN_PIN,0);
  analogWrite(RIN_PIN,0);
}



