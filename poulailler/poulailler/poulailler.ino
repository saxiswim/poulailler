#include <DS3231.h>
#include <config.h>
#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


//création du servo
Servo Moteur;

//création de l'écran lcd
LiquidCrystal_I2C lcd(0x27,20,4);  //ecran lcd 


//trame contenants les infos IR
uint32_t trameIR;
uint16_t trameBTN;

//création du module RTC
DS3231 rtc(SDA, SCL);


//position Moteur
int positionMoteur = 0; 

//booléan permmettant de savoir si on a un ecran de veille ou non
bool Veille = true;

//boléan permttant de savoir si le poullailer est ouvert ou fermé 
bool Open = false;
bool Close = true;

//horaire d'ouverture et de fermeture
int OpenMinute = 50;
int CloseMinute = 45;
int OpenHeure = 7;
int CloseHeure = 21;

//temps
Time t;

void setup() {
  Serial.begin(9600);
  
  //init rtc
  rtc.begin();
  rtc.setTime(22,1,0);
  rtc.setDate(12,7,2021);

  //init lcd
  lcd.init();
  lcd.backlight();

  //init IrReceiver
  IrReceiver.begin(7, DISABLE_LED_FEEDBACK);

  //init moteur
  Moteur.attach(9);
  Moteur.write(0);
}

void loop() {
  t = rtc.getTime();

  if (t.hour == OpenHeure and t.min == OpenMinute and t.sec == 0 and not(Open)) {
    Openmoteur(Moteur, lcd);
  }
  if (Veille) {
    AffichageVeille(lcd,rtc, Moteur); 
  }
   
  if (IrReceiver.decode()){
    trameIR = IrReceiver.decodedIRData.decodedRawData;
    trameBTN = (uint16_t) (trameIR>>16);
    menu(get_code(trameBTN), lcd,rtc, Moteur);
    IrReceiver.resume();
  }
}

void affichageOpen(LiquidCrystal_I2C lcd){
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("OUVERTURE");
  lcd.setCursor(9,1);
  lcd.print("DU");
  lcd.setCursor(5,2);
  lcd.print("POULAILLER");
  delay(100);
  lcd.clear();
}

void affichageClose(LiquidCrystal_I2C lcd){
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("FERMETURE");
  lcd.setCursor(9,1);
  lcd.print("DU");
  lcd.setCursor(5,2);
  lcd.print("POULAILLER");
  delay(100);
  lcd.clear();
}

void Erreur(LiquidCrystal_I2C lcd){
  lcd.clear();
  lcd.setCursor(7,0);
  lcd.print("ERREUR");
  lcd.setCursor(0,1);
  lcd.print("--------------------");
  
  lcd.setCursor(0,3);
  lcd.print("--------------------");
  delay(3000);
  lcd.clear();
}

void Openmoteur(Servo moteur, LiquidCrystal_I2C lcd){
  if (Open == false and positionMoteur == 0){
    moteur.write(180);
    affichageOpen(lcd);
    delay(1000);  
    Open = true;
    Close = false;
    positionMoteur = 180;
  }
  else {
    Erreur(lcd);
  }
  Veille = true;
}


void Closemoteur(Servo moteur, LiquidCrystal_I2C lcd){
  if (Close == false and positionMoteur == 180) {
    moteur.write(0);
    affichageClose(lcd);
    delay(2);
    Close = true;
    Open = false;
    positionMoteur = 0; 
  }
  else {
    Erreur(lcd);
  }
  Veille = true;
}

void printDate(LiquidCrystal_I2C lcd,DS3231 rtc) {
  char * TIME;
  char * DATE;

  
  TIME = rtc.getTimeStr();
  DATE = rtc.getDateStr();
  
  lcd.setCursor(0,0);
  lcd.print(DATE); 
  lcd.setCursor(0,1);
  lcd.print(TIME);  
  lcd.setCursor(0,2);
  lcd.print("OpenTime -> ");
  lcd.print(OpenHeure);
  lcd.print(":");
  lcd.print(OpenMinute);
  lcd.setCursor(0,3);
  lcd.print("CloseTime -> ");
  lcd.print(CloseHeure);
  lcd.print(":");
  lcd.print(CloseMinute);
  if (Open){
    lcd.setCursor(15,0);
    lcd.print(" OPEN");
  }
  else {
    lcd.setCursor(15,0);
    lcd.print("CLOSE");
  }

  
}

void AffichageVeille(LiquidCrystal_I2C lcd,DS3231 rtc, Servo moteur) {
  printDate(lcd,rtc);
  if (IrReceiver.decode()){
    Serial.println("entre");
    trameIR = IrReceiver.decodedIRData.decodedRawData;
    trameBTN = (uint16_t) (trameIR>>16);
    menu(get_code(trameBTN), lcd, rtc, moteur);
    IrReceiver.resume();
  }
}

void EncoursDeConstruction(LiquidCrystal_I2C lcd,DS3231 rtc){
  lcd.clear();
  lcd.setCursor(9,0);
  lcd.print("EN");
  lcd.setCursor(7,1);
  lcd.print("COURS");
  lcd.setCursor(9,2);
  lcd.print("DE");
  lcd.setCursor(5,3);
  lcd.print("CONCEPTION");
  delay(1000);
  lcd.clear();
  Veille = true;
}

void commandeNonValide(LiquidCrystal_I2C lcd,DS3231 rtc) {
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("COMMANDE");
  lcd.setCursor(8,1);
  lcd.print("NON");
  lcd.setCursor(7,2);
  lcd.print("VALIDE");
  delay(1000);
  lcd.clear();
  Veille = true;
}

void menu(String BTN, LiquidCrystal_I2C lcd,DS3231 rtc, Servo moteur){
  Veille = false;
  //si bouton 1
  if (BTN.equals("1")) {
    updateDate(rtc,lcd);
  }
  //si bouton 2
  else if (BTN.equals("2")){
    updateTime(rtc, lcd);
  }
  //sinon
  else {
    if (BTN.equals("3")){
      modifLeve(rtc, lcd);
    }
    else if (BTN.equals("4")){
      modifCouche(rtc,lcd);
    }
    else {
      if (BTN.equals("5")){
        Openmoteur(Moteur, lcd);
      }
      else if (BTN.equals("6")){
        Closemoteur(Moteur, lcd);
      }
      else {
        commandeNonValide(lcd, rtc);     
      }
    }
  }
}

String get_code(uint16_t codeBouton){  
      switch(codeBouton) {
        case 0xBA45:
          return "power";
          break;
          
        case 0xB946:
          return "Vol+";
          break;
          
        case 0xB847:
          return "Stop";
          break;
          
        case 0xBB44:
          return "|<<";
          break;
          
        case 0xBF40:
          return ">||";
          break;
    
        case 0xBC43:
          return ">>|";
          break;
          
        case 0xF807:
          return "v";
          break;
    
        case 0xEA15:
          return "Vol-";
          break;
          
        case 0xF609:
          return "^";
          break;
          
        case 0xE916:
          return "0";
          break;
          
        case 0xE619:
          return "EQ";
          break;
          
        case 0xF20D:
          return "ST/REPT";
          break;
          
        case 0xF30C:
          return "1";
          break;
          
        case 0xE718:
          return "2";
          break;
          
        case 0xA15E:
          return "3";
          break;
          
        case 0xF708:
          return "4";
          break;
          
        case 0xE31C:
          return "5";
          break;
          
        case 0xA55A:
          return "6";
          break;
          
        case 0xBD42:
          return "7";
          break;
          
        case 0xAD52:
          return "8";
          break;
          
        case 0xB54A:
          return "9";
          break;
    
        default:
          return "error";
          break;
      }
}


void enteteUpdateDate(LiquidCrystal_I2C lcd) {
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Modif Date");
    lcd.setCursor(0,1);
    lcd.print("--------------------");
}

void enteteUpdateTIME(LiquidCrystal_I2C lcd) {
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Modif Time");
    lcd.setCursor(0,1);
    lcd.print("--------------------");
}


void enteteLeve(LiquidCrystal_I2C lcd) {
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("Modif Levé");
    lcd.setCursor(0,1);
    lcd.print("--------------------");
}

void enteteCouche(LiquidCrystal_I2C lcd) {
    lcd.clear();
    lcd.setCursor(6,0);
    lcd.print("Modif Couche");
    lcd.setCursor(0,1);
    lcd.print("--------------------");
}


void modifLeve(DS3231 rtc, LiquidCrystal_I2C lcd){
    enteteLeve(lcd);
    int heure = 0;
    int openMinute = 0;
    int tmp = 0;
    lcd.setCursor(8,2);
    lcd.print("HEURE");
    while (true) {
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(heure);
      lcd.print(">");
      tmp = updateCompteur(rtc, heure);
      if (tmp == 10000){
        break;
      }
      else {
        heure = tmp;
      }
    }
    tmp = 0;
    lcd.clear();
    enteteLeve(lcd);
    lcd.setCursor(7,2);
    lcd.print("MINUTE");
    while (true) {
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(openMinute);
      lcd.print(">");
      tmp = updateCompteur(rtc, openMinute);
      if (tmp == 10000){
        break;
      }
      else {
        openMinute = tmp;
      }
    }
    delay(500);
    lcd.clear();
    OpenMinute = openMinute;
    OpenHeure = heure; 
    Veille = true;
}


void modifCouche(DS3231 rtc, LiquidCrystal_I2C lcd){
    enteteCouche(lcd);
    int closeheure = 0;
    int closeMinute = 0; 
    int tmp = 0;
    lcd.setCursor(8,2);
    lcd.print("HEURE");
    while (true) {
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(closeheure);
      lcd.print(">");
      tmp = updateCompteur(rtc, closeheure);
      if (tmp == 10000){
        break;
      }
      else {
        closeheure = tmp;
      }
    }
    tmp = 0;
    lcd.clear();
    enteteCouche(lcd);
    lcd.setCursor(7,2);
    lcd.print("MINUTE");
    while (true) {
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(closeMinute);
      lcd.print(">");
      tmp = updateCompteur(rtc, closeMinute);
      if (tmp == 10000){
        break;
      }
      else {
        closeMinute = tmp;
      }
    }
    delay(500);
    lcd.clear();
    CloseMinute = closeMinute;
    CloseHeure = closeheure; 
    Veille = true;
}


void updateDate(DS3231 rtc, LiquidCrystal_I2C lcd){
    enteteUpdateDate(lcd);
    lcd.setCursor(8,2);
    lcd.print("JOUR");
    int jour = 0;
    int mois = 0;
    int ans = 2021;
    int tmp = 0;
    while (true) {
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(jour);
      lcd.print(">");
      tmp = updateCompteur(rtc, jour);
      if (tmp == 10000){
        break;
      }
      else {
        jour = tmp;
      }
    }
    tmp = 0;
    lcd.clear();
    enteteUpdateDate(lcd);
    lcd.setCursor(8,2);
    lcd.print("MOIS");
    while(true){
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(mois);
      lcd.print(">");
      tmp = updateCompteur(rtc, mois);
      if (tmp == 10000){
        break;
      }
      else {
        mois = tmp;
      }
    }
    tmp = 0;
    lcd.clear();
    enteteUpdateDate(lcd);
    lcd.setCursor(7,2);
    lcd.print("ANNEE");
    while(true){
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(ans);
      lcd.print(">");
      tmp = updateCompteur(rtc, ans);
      if (tmp == 10000){
        break;
      }
      else {
        ans = tmp;
      }
    }   
     
    delay(1000);
    rtc.setDate(jour,mois,ans);
    lcd.clear();
    Veille = true;
    //rtc.setDate(jour,mois,ans);
}


void updateTime(DS3231 rtc, LiquidCrystal_I2C lcd){
    enteteUpdateTIME(lcd);
    lcd.setCursor(8,2);
    lcd.print("HEURE");
    int HEURE = 0;
    int MINUTE = 0;
    int tmp = 0;
    while (true) {
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(HEURE);
      lcd.print(">");
      tmp = updateCompteur(rtc, HEURE);
      if (tmp == 10000){
        break;
      }
      else {
        HEURE = tmp;
      }
    }
    tmp = 0;
    lcd.clear();
    enteteUpdateTIME(lcd);
    lcd.setCursor(7,2);
    lcd.print("MINUTE");
    while(true){
      lcd.setCursor(8,3);
      lcd.print("<");
      lcd.print(MINUTE);
      lcd.print(">");
      tmp = updateCompteur(rtc, MINUTE);
      if (tmp == 10000){
        break;
      }
      else {
        MINUTE = tmp;
      }
    }
    delay(1000);
    rtc.setTime(HEURE, MINUTE, 0);
    lcd.clear();
    Veille = true;
    //rtc.setDate(jour,mois,ans);
}



int updateCompteur(DS3231 rtc, int cpt){
  //compteur de départ
  //si on reçoit un signal du type vol+ alors on incrémente le compteur
  //si on reçoit un compteur du type vol- alors on diminue de 1 le compteur
  if (IrReceiver.decode()){
    trameIR = IrReceiver.decodedIRData.decodedRawData;
    trameBTN = (uint16_t) (trameIR>>16);
    //si la trame indique vol+
    if (get_code(trameBTN).equals("Vol+")){
      cpt++;
    }
    //si la trame indique vol-
    if (get_code(trameBTN).equals("Vol-")){
      cpt--;
    }
    //sinon
    //si la trame indique EQ alors on retourne -1
    if (get_code(trameBTN).equals("EQ")){
      cpt = 10000;
    }
    //sinon on ne fait rien 
  //on résume pour pouvoir repérer un nouveau signal IR
  IrReceiver.resume();
  }
  return cpt;
}
