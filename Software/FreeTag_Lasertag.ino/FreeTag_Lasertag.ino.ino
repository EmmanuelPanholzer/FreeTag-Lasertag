#include<LiquidCrystal.h> // LCD Libary wird geladen
#include <Wtv020sd16p.h>  // Soundmodul Libary wird geladen


//Spielvariablen
int Health;    //Speicherwert fuer die Gesundheit
int Life;      // Speicherwert fuer die Leben
char* Player;   // Speciherwert fuer den Spielername
int PlayerID;    // Speicherwert fuer die Spielernummer
int Team;       // Speicherwert fuer die Teamnummer
int Damage;     // Speicherwert fuer die Schaden den man ausgibt
int Ammo;      // Speicherwert fuer die Magazingroesse 
int Clips;     // Speicherwert fuer die Magazine
int clips;     // Speicherwert fuer die momentanen Magazine
int ammoUse;   // Speicherwert fuer den Munitionsverrauch pro SchuÃŸ
unsigned int shots;  // Speicherwert fuer die Abgegebenen Schuessee im Spiel
int ammo;      // Speicherwert fuer dei Momentane Munition
unsigned int respawnTime; //Zeitdauer nach Tod, wenn wieder gerespawnt wird
boolean friendlyFire;  // Speicherwert fuer Friendly Fire
boolean fullAuto; // Speicherwert ob man full Auto nutzen möchte
boolean clipUse;  //Speicherwert fuer Magazinverbrauch
boolean respawn;  // Speicherwert ob man nach endgueltigen tod nochmal respawnen kann
boolean shortRange; // Speicherwert ob man die kürzere Reichweite nutzen möchte
boolean indoorMode; //Speicherwert ob man Indoor Spielt(kurze reichweite) oder Outdoor
boolean sound;    //Speicherwert ob man mit Sounds Spielen moechte

//Spielmode und Adminvariablen
boolean Healed;  //Speicherwert ob jemand sich pro Leben bereits geheilt hat
boolean AdminKill; //Speicherwert ob man vom Admin ausgeschaltet wurde
boolean Pause;     // unterbricht das Spiel(also Pause)
boolean Game;     // Speicherwert ob das Spiel lÃ¤uft
boolean DataRecv; // Speicherwert ob das die Scoredaten erfolgreich gesendet wurden
boolean ctf;      // Speicherwert ob Capture the Flag gespielt wird
boolean Flag;     // Speicherwert das man die Flage hat
boolean flagFire;  // Speicherwert ob man wÃ¤hren man die Flagge hat, Schiessen kann

//Hardwareinfos
int sendLedShortIndoor = 37;  //IR-Led(kurze Reichweite)für Indoor ist auf Pin 37
int sendLedIndoor = 39;   //IR-Led(lange Reichweite)für Indoor ist auf Pin 39
int hitLed = 40;      // hitLed auf 40
int sendLedShort = 41; //IR-Led(kurze Reichweite)ist auf Pin 41
int recvPin = 42;      //Empfaengerled ist auf Pin 42
int sendLed = 43;     //IR-Led(lange Reichweite) ist auf Pin 43
int triggerPin = 44;    //Abzugschalter ist auf Pin 44
int reloadPin = 46;   // Nachladeknopf ist auf Pin 46
int gamePin = 48;     //Gamebutton ist auf Pin 48
int muzzleLed = 50;     // MÃ¼ndungsled ist auf Pin 50
int lcdBackroundLight = 10; //LCD Hintergrundbeleuchtung ist auf Pin 10

int dataPin=14;           //Soundmodul Pin 10 liegt an Pin 15 Arduino      
int clockPin=16;          //Soundmodul Pin 7 liegt an Pin 15 Arduino
int resetPin=15;          //Soundmodul Pin 1 liegt an Pin 15 Arduino
int busyPin=20;           //Soundmodul Pin 15 liegt an Pin 15 Arduino
int noSoundPin=17;        //die Soundunterdrueckung fuer das Soundmodul liegt an Pin 17

int startBit=2400;        //Speicherwert Startsequenz
int endBit=3000;          //Speicherwert Endsequenz = 3000
int oneBit=1200;          //Speicherwert 1 Bit
int zeroBit=600;         //Speicherwert 0 Bit

int divisorstart=37;    // Wert zum korigieren des Zeitzaehlfehlers des Arduinos beim Startbit
int divisorone=39;      // Wert zum korigieren des Zeitzaehlfehlers des Arduinos beim 1 bit
int divisorzero=42;     // Wert zum korigieren des Zeitzaehlfehlers des Arduinos beim 0 bit

boolean pressedReloadPin;
boolean pressedTriggerPin;
boolean hited;
boolean outOfClips;
boolean gameOver;
boolean outOfAmmo;
boolean incSerial;
boolean CtfLed;
boolean flagSend;
boolean dead;
boolean hitLedOn;
boolean stateBusyPinHigh;
LiquidCrystal lcd(12,11,5,4,3,2);   // Gibt die LCD Pins an die Libary
Wtv020sd16p wtv020sd16p(resetPin,clockPin,dataPin,busyPin); // Gibt die SoundModul Pins an die Libary


//Ãœbertragungsvariablen
int length;
int bite1[8];         //zwischenspeicher fuer bite 1!!!! noch umrechnen und verbessern
int bite2[6];         //zwischenspeicher fuer bite 2!!!! noch umrechnen und verbessern
int recieved1[8];     // zwischenspeicher fuer Eingehendes SchuÃŸ int 1
int recieved2[6];     // zwischenspeicher fuer Eingehendes SchuÃŸ Byte 2
int byteData1[8];     // zwischenspeicher fuer das Versenden von Datenbyte 1
int byteData2[8];     // zwischenspeicher fuer das Versenden von Datenbyte 2
int byteData3[8];     // zwischenspeicher fuer das Versenden von Datenbyte 3
int recievedData1[8]; // zwischenspeicher fuer Eingehendes Datenpacket Byte 1
int recievedData2[8]; // zwischenspeicher fuer Eingehendes Datenpacket Byte 2
int recievedData3[8]; // zwischenspeicher fuer Eingehendes Datenpacket Byte 3


//Speichervariablen fÃ¼r die Auswertung
unsigned int enemyHits[65];    //Speicherwert fÃ¼r den kasierten Schaden der Einzelnen Gegner
unsigned int usedAmmo;         // gesamt verbrauchte Munition
char enemyName[65][12]; // Speicherwert der Namen aller Spieler
int flagNumber;      //Speicherwert fÃ¼r die Nummer die die Flagge traegt
unsigned long currentMillis;
unsigned long currentMillisCtf;
int fireCount;


void setup()
{
  Serial.begin(9600);
  wtv020sd16p.reset();

  enemyName[1][0]='M';enemyName[1][1]='a';enemyName[1][2]='n';enemyName[1][3]='z';enemyName[1][4]='i';enemyName[1][5]=' ';enemyName[1][6]=' ';enemyName[1][7]=' ';enemyName[1][8]=' ';enemyName[1][9]=' ';enemyName[1][10]=' ';enemyName[1][11]='\0';  
  enemyName[2][0]='R';enemyName[2][1]='o';enemyName[2][2]='l';enemyName[2][3]='i';enemyName[2][4]=' ';enemyName[2][5]=' ';enemyName[2][6]=' ';enemyName[2][7]=' ';enemyName[2][8]=' ';enemyName[2][9]=' ';enemyName[2][10]=' ';enemyName[2][11]='\0';
  enemyName[3][0]='M';enemyName[3][1]='a';enemyName[3][2]='x';enemyName[3][3]=' ';enemyName[3][4]=' ';enemyName[3][5]=' ';enemyName[3][6]=' ';enemyName[3][7]=' ';enemyName[3][8]=' ';enemyName[3][9]=' ';enemyName[3][10]=' ';enemyName[3][11]='\0';
  enemyName[4][0]='S';enemyName[4][1]='i';enemyName[4][2]='g';enemyName[4][3]='i';enemyName[4][4]=' ';enemyName[4][5]=' ';enemyName[4][6]=' ';enemyName[4][7]=' ';enemyName[4][8]=' ';enemyName[4][9]=' ';enemyName[4][10]=' ';enemyName[4][11]='\0';
  enemyName[5][0]='K';enemyName[5][1]='o';enemyName[5][2]='n';enemyName[5][3]='s';enemyName[5][4]='t';enemyName[5][5]='l';enemyName[5][6]=' ';enemyName[5][7]=' ';enemyName[5][8]=' ';enemyName[5][9]=' ';enemyName[5][10]=' ';enemyName[5][11]='\0';
  enemyName[6][0]='H';enemyName[6][1]='a';enemyName[6][2]='u';enemyName[6][3]='n';enemyName[6][4]='s';enemyName[6][5]=' ';enemyName[6][6]=' ';enemyName[6][7]=' ';enemyName[6][8]=' ';enemyName[6][9]=' ';enemyName[6][10]=' ';enemyName[6][11]='\0';
  enemyName[7][0]='J';enemyName[7][1]='o';enemyName[7][2]='n';enemyName[7][3]='n';enemyName[7][4]='y';enemyName[7][5]=' ';enemyName[7][6]=' ';enemyName[7][7]=' ';enemyName[7][8]=' ';enemyName[7][9]=' ';enemyName[7][10]=' ';enemyName[7][11]='\0';  
  enemyName[8][0]='C';enemyName[8][1]='h';enemyName[8][2]='e';enemyName[8][3]='l';enemyName[8][4]='e';enemyName[8][5]='s';enemyName[8][6]='t';enemyName[8][7]='o';enemyName[8][8]='r';enemyName[8][9]=' ';enemyName[8][10]=' ';enemyName[8][11]='\0';
  enemyName[9][0]='W';enemyName[9][1]='u';enemyName[9][2]='r';enemyName[9][3]='s';enemyName[9][4]='t';enemyName[9][5]=' ';enemyName[9][6]=' ';enemyName[9][7]=' ';enemyName[9][8]=' ';enemyName[9][9]=' ';enemyName[9][10]=' ';enemyName[9][11]='\0';
  
  enemyName[10][0]='D';enemyName[10][1]='e';enemyName[10][2]='i';enemyName[10][3]=' ';enemyName[10][4]='M';enemyName[10][5]='u';enemyName[10][6]='m';enemyName[10][7]=' ';enemyName[10][8]=' ';enemyName[10][9]=' ';enemyName[10][10]=' ';enemyName[10][11]='\0';
  enemyName[11][0]='D';enemyName[11][1]='a';enemyName[11][2]='v';enemyName[11][3]='e';enemyName[11][4]=' ';enemyName[11][5]=' ';enemyName[11][6]=' ';enemyName[11][7]=' ';enemyName[11][8]=' ';enemyName[11][9]=' ';enemyName[11][10]=' ';enemyName[11][11]='\0';
  enemyName[12][0]='S';enemyName[12][1]='t';enemyName[12][2]='e';enemyName[12][3]='p';enemyName[12][4]='h';enemyName[12][5]=' ';enemyName[12][6]=' ';enemyName[12][7]=' ';enemyName[12][8]=' ';enemyName[12][9]=' ';enemyName[12][10]=' ';enemyName[12][11]='\0';
  enemyName[13][0]='M';enemyName[13][1]='a';enemyName[13][2]='c';enemyName[13][3]='i';enemyName[13][4]=' ';enemyName[13][5]=' ';enemyName[13][6]=' ';enemyName[13][7]=' ';enemyName[13][8]=' ';enemyName[13][9]=' ';enemyName[13][10]=' ';enemyName[13][11]='\0';
  enemyName[14][0]='W';enemyName[14][1]='i';enemyName[14][2]='n';enemyName[14][3]='d';enemyName[14][4]='o';enemyName[14][5]='w';enemyName[14][6]='s';enemyName[14][7]=' ';enemyName[14][8]=' ';enemyName[14][9]=' ';enemyName[14][10]=' ';enemyName[14][11]='\0';
  enemyName[15][0]='H';enemyName[15][1]='e';enemyName[15][2]='n';enemyName[15][3]='d';enemyName[15][4]='l';enemyName[15][5]=' ';enemyName[15][6]=' ';enemyName[15][7]=' ';enemyName[15][8]=' ';enemyName[15][9]=' ';enemyName[15][10]=' ';enemyName[15][11]='\0';
  enemyName[16][0]='W';enemyName[16][1]='u';enemyName[16][2]='r';enemyName[16][3]='s';enemyName[16][4]='t';enemyName[16][5]=' ';enemyName[16][6]=' ';enemyName[16][7]=' ';enemyName[16][8]=' ';enemyName[16][9]=' ';enemyName[16][10]=' ';enemyName[16][11]='\0';
  enemyName[17][0]='G';enemyName[17][1]='r';enemyName[17][2]='a';enemyName[17][3]='v';enemyName[17][4]='o';enemyName[17][5]='g';enemyName[17][6]='e';enemyName[17][7]='l';enemyName[17][8]=' ';enemyName[17][9]=' ';enemyName[17][10]=' ';enemyName[17][11]='\0';
  enemyName[18][0]='R';enemyName[18][1]='o';enemyName[18][2]='c';enemyName[18][3]='k';enemyName[18][4]='f';enemyName[18][5]='o';enemyName[18][6]='r';enemyName[18][7]='d';enemyName[18][8]=' ';enemyName[18][9]=' ';enemyName[18][10]=' ';enemyName[18][11]='\0';
  enemyName[19][0]='A';enemyName[19][1]='p';enemyName[19][2]='s';enemyName[19][3]=' ';enemyName[19][4]=' ';enemyName[19][5]=' ';enemyName[19][6]=' ';enemyName[19][7]=' ';enemyName[19][8]=' ';enemyName[19][9]=' ';enemyName[19][10]=' ';enemyName[19][11]='\0';
  
  enemyName[20][0]='H';enemyName[20][1]='i';enemyName[20][2]='m';enemyName[20][3]='m';enemyName[20][4]='e';enemyName[20][5]='l';enemyName[20][6]=' ';enemyName[20][7]=' ';enemyName[20][8]=' ';enemyName[20][9]=' ';enemyName[20][10]=' ';enemyName[20][11]='\0';
  enemyName[21][0]='H';enemyName[21][1]='a';enemyName[21][2]='l';enemyName[21][3]='l';enemyName[21][4]='e';enemyName[21][5]=' ';enemyName[21][6]=' ';enemyName[21][7]=' ';enemyName[21][8]=' ';enemyName[21][9]=' ';enemyName[21][10]=' ';enemyName[21][11]='\0';
  enemyName[22][0]='S';enemyName[22][1]='e';enemyName[22][2]='p';enemyName[22][3]='h';enemyName[22][4]='i';enemyName[22][5]='r';enemyName[22][6]='o';enemyName[22][7]='t';enemyName[22][8]=' ';enemyName[22][9]=' ';enemyName[22][10]=' ';enemyName[22][11]='\0';
  enemyName[23][0]='C';enemyName[23][1]='l';enemyName[23][2]='o';enemyName[23][3]='u';enemyName[23][4]='d';enemyName[23][5]=' ';enemyName[23][6]=' ';enemyName[23][7]=' ';enemyName[23][8]=' ';enemyName[23][9]=' ';enemyName[23][10]=' ';enemyName[23][11]='\0';
  enemyName[24][0]='S';enemyName[24][1]='v';enemyName[24][2]='e';enemyName[24][3]='n';enemyName[24][4]=' ';enemyName[24][5]=' ';enemyName[24][6]=' ';enemyName[24][7]=' ';enemyName[24][8]=' ';enemyName[24][9]=' ';enemyName[24][10]=' ';enemyName[24][11]='\0';
  enemyName[25][0]='H';enemyName[25][1]='a';enemyName[25][2]='u';enemyName[25][3]='s';enemyName[25][4]=' ';enemyName[25][5]=' ';enemyName[25][6]=' ';enemyName[25][7]=' ';enemyName[25][8]=' ';enemyName[25][9]=' ';enemyName[25][10]=' ';enemyName[25][11]='\0';
  enemyName[26][0]='K';enemyName[26][1]='i';enemyName[26][2]='r';enemyName[26][3]='c';enemyName[26][4]='h';enemyName[26][5]='e';enemyName[26][6]=' ';enemyName[26][7]=' ';enemyName[26][8]=' ';enemyName[26][9]=' ';enemyName[26][10]=' ';enemyName[26][11]='\0';
  enemyName[27][0]='A';enemyName[27][1]='u';enemyName[27][2]='t';enemyName[27][3]='o';enemyName[27][4]=' ';enemyName[27][5]=' ';enemyName[27][6]=' ';enemyName[27][7]=' ';enemyName[27][8]=' ';enemyName[27][9]=' ';enemyName[27][10]=' ';enemyName[27][11]='\0';
  enemyName[28][0]='C';enemyName[28][1]='l';enemyName[28][2]='o';enemyName[28][3]='u';enemyName[28][4]='d';enemyName[28][5]=' ';enemyName[28][6]=' ';enemyName[28][7]=' ';enemyName[28][8]=' ';enemyName[28][9]=' ';enemyName[28][10]=' ';enemyName[23][11]='\0';
  enemyName[29][0]='R';enemyName[29][1]='e';enemyName[29][2]='m';enemyName[29][3]='i';enemyName[29][4]=' ';enemyName[29][5]=' ';enemyName[29][6]=' ';enemyName[29][7]=' ';enemyName[29][8]=' ';enemyName[29][9]=' ';enemyName[29][10]=' ';enemyName[29][11]='\0';
  
  enemyName[30][0]='L';enemyName[30][1]='i';enemyName[30][2]='n';enemyName[30][3]='u';enemyName[30][4]='x';enemyName[30][5]=' ';enemyName[30][6]=' ';enemyName[30][7]=' ';enemyName[30][8]=' ';enemyName[30][9]=' ';enemyName[30][10]=' ';enemyName[30][11]='\0';
  enemyName[31][0]='H';enemyName[31][1]='a';enemyName[31][2]='u';enemyName[31][3]='s';enemyName[31][4]='m';enemyName[31][5]='e';enemyName[31][6]='i';enemyName[31][7]='s';enemyName[31][8]='t';enemyName[31][9]='e';enemyName[31][10]='r';enemyName[31][11]='\0';
  enemyName[32][0]='T';enemyName[32][1]='u';enemyName[32][2]='f';enemyName[32][3]='f';enemyName[32][4]=' ';enemyName[32][5]=' ';enemyName[32][6]=' ';enemyName[32][7]=' ';enemyName[32][8]=' ';enemyName[32][9]=' ';enemyName[32][10]=' ';enemyName[32][11]='\0';
  enemyName[33][0]='W';enemyName[33][1]='u';enemyName[33][2]='t';enemyName[33][3]='o';enemyName[33][4]='k';enemyName[33][5]=' ';enemyName[33][6]=' ';enemyName[33][7]=' ';enemyName[33][8]=' ';enemyName[33][9]=' ';enemyName[33][10]=' ';enemyName[33][11]='\0';
  enemyName[34][0]='L';enemyName[34][1]='i';enemyName[34][2]='n';enemyName[34][3]='u';enemyName[34][4]='x';enemyName[34][5]=' ';enemyName[34][6]=' ';enemyName[34][7]=' ';enemyName[34][8]=' ';enemyName[34][9]=' ';enemyName[34][10]=' ';enemyName[34][11]='\0';
  enemyName[35][0]='H';enemyName[35][1]='a';enemyName[35][2]='u';enemyName[35][3]='s';enemyName[35][4]='m';enemyName[35][5]='e';enemyName[35][6]='i';enemyName[35][7]='s';enemyName[35][8]='t';enemyName[35][9]='e';enemyName[35][10]='r';enemyName[35][11]='\0';
  enemyName[36][0]='T';enemyName[36][1]='u';enemyName[36][2]='f';enemyName[36][3]='f';enemyName[36][4]=' ';enemyName[36][5]=' ';enemyName[36][6]=' ';enemyName[36][7]=' ';enemyName[36][8]=' ';enemyName[36][9]=' ';enemyName[36][10]=' ';enemyName[36][11]='\0';
  enemyName[37][0]='W';enemyName[37][1]='u';enemyName[37][2]='t';enemyName[37][3]='o';enemyName[37][4]='k';enemyName[37][5]=' ';enemyName[37][6]=' ';enemyName[37][7]=' ';enemyName[37][8]=' ';enemyName[37][9]=' ';enemyName[37][10]=' ';enemyName[37][11]='\0';
  enemyName[38][0]='T';enemyName[38][1]='u';enemyName[38][2]='f';enemyName[38][3]='f';enemyName[38][4]=' ';enemyName[38][5]=' ';enemyName[38][6]=' ';enemyName[38][7]=' ';enemyName[38][8]=' ';enemyName[38][9]=' ';enemyName[38][10]=' ';enemyName[38][11]='\0';
  enemyName[39][0]='W';enemyName[39][1]='u';enemyName[39][2]='t';enemyName[39][3]='o';enemyName[39][4]='k';enemyName[39][5]=' ';enemyName[39][6]=' ';enemyName[39][7]=' ';enemyName[39][8]=' ';enemyName[39][9]=' ';enemyName[39][10]=' ';enemyName[39][11]='\0';

  enemyName[40][0]='L';enemyName[40][1]='i';enemyName[40][2]='n';enemyName[40][3]='u';enemyName[40][4]='x';enemyName[40][5]=' ';enemyName[40][6]=' ';enemyName[40][7]=' ';enemyName[40][8]=' ';enemyName[40][9]=' ';enemyName[40][10]=' ';enemyName[40][11]='\0';
  enemyName[41][0]='H';enemyName[41][1]='a';enemyName[41][2]='u';enemyName[41][3]='s';enemyName[41][4]='m';enemyName[41][5]='e';enemyName[41][6]='i';enemyName[41][7]='s';enemyName[41][8]='t';enemyName[41][9]='e';enemyName[41][10]='r';enemyName[41][11]='\0';
  enemyName[42][0]='T';enemyName[42][1]='u';enemyName[42][2]='f';enemyName[42][3]='f';enemyName[42][4]=' ';enemyName[42][5]=' ';enemyName[42][6]=' ';enemyName[42][7]=' ';enemyName[42][8]=' ';enemyName[42][9]=' ';enemyName[42][10]=' ';enemyName[42][11]='\0';
  enemyName[43][0]='W';enemyName[43][1]='u';enemyName[43][2]='t';enemyName[43][3]='o';enemyName[43][4]='k';enemyName[43][5]=' ';enemyName[43][6]=' ';enemyName[43][7]=' ';enemyName[43][8]=' ';enemyName[43][9]=' ';enemyName[43][10]=' ';enemyName[43][11]='\0';
  enemyName[44][0]='L';enemyName[44][1]='i';enemyName[44][2]='n';enemyName[44][3]='u';enemyName[44][4]='x';enemyName[44][5]=' ';enemyName[44][6]=' ';enemyName[44][7]=' ';enemyName[44][8]=' ';enemyName[44][9]=' ';enemyName[44][10]=' ';enemyName[44][11]='\0';
  enemyName[45][0]='H';enemyName[45][1]='a';enemyName[45][2]='u';enemyName[45][3]='s';enemyName[45][4]='m';enemyName[45][5]='e';enemyName[45][6]='i';enemyName[45][7]='s';enemyName[45][8]='t';enemyName[45][9]='e';enemyName[45][10]='r';enemyName[45][11]='\0';
  enemyName[46][0]='T';enemyName[46][1]='u';enemyName[46][2]='f';enemyName[46][3]='f';enemyName[46][4]=' ';enemyName[46][5]=' ';enemyName[46][6]=' ';enemyName[46][7]=' ';enemyName[46][8]=' ';enemyName[46][9]=' ';enemyName[46][10]=' ';enemyName[46][11]='\0';
  enemyName[47][0]='W';enemyName[47][1]='u';enemyName[47][2]='t';enemyName[47][3]='o';enemyName[47][4]='k';enemyName[47][5]=' ';enemyName[47][6]=' ';enemyName[47][7]=' ';enemyName[47][8]=' ';enemyName[47][9]=' ';enemyName[47][10]=' ';enemyName[47][11]='\0';
  enemyName[48][0]='T';enemyName[48][1]='u';enemyName[48][2]='f';enemyName[48][3]='f';enemyName[48][4]=' ';enemyName[48][5]=' ';enemyName[48][6]=' ';enemyName[48][7]=' ';enemyName[48][8]=' ';enemyName[48][9]=' ';enemyName[48][10]=' ';enemyName[48][11]='\0';
  enemyName[49][0]='W';enemyName[49][1]='u';enemyName[49][2]='t';enemyName[49][3]='o';enemyName[49][4]='k';enemyName[49][5]=' ';enemyName[49][6]=' ';enemyName[49][7]=' ';enemyName[49][8]=' ';enemyName[49][9]=' ';enemyName[49][10]=' ';enemyName[49][11]='\0';

  enemyName[50][0]='L';enemyName[50][1]='i';enemyName[50][2]='n';enemyName[50][3]='u';enemyName[50][4]='x';enemyName[50][5]=' ';enemyName[50][6]=' ';enemyName[50][7]=' ';enemyName[50][8]=' ';enemyName[50][9]=' ';enemyName[50][10]=' ';enemyName[50][11]='\0';
  enemyName[51][0]='H';enemyName[51][1]='a';enemyName[51][2]='u';enemyName[51][3]='s';enemyName[51][4]='m';enemyName[51][5]='e';enemyName[51][6]='i';enemyName[51][7]='s';enemyName[51][8]='t';enemyName[51][9]='e';enemyName[51][10]='r';enemyName[51][11]='\0';
  enemyName[52][0]='T';enemyName[52][1]='u';enemyName[52][2]='f';enemyName[52][3]='f';enemyName[52][4]=' ';enemyName[52][5]=' ';enemyName[52][6]=' ';enemyName[52][7]=' ';enemyName[52][8]=' ';enemyName[52][9]=' ';enemyName[52][10]=' ';enemyName[52][11]='\0';
  enemyName[53][0]='W';enemyName[53][1]='u';enemyName[53][2]='t';enemyName[53][3]='o';enemyName[53][4]='k';enemyName[53][5]=' ';enemyName[53][6]=' ';enemyName[53][7]=' ';enemyName[53][8]=' ';enemyName[53][9]=' ';enemyName[53][10]=' ';enemyName[53][11]='\0';
  enemyName[54][0]='L';enemyName[54][1]='i';enemyName[54][2]='n';enemyName[54][3]='u';enemyName[54][4]='x';enemyName[54][5]=' ';enemyName[54][6]=' ';enemyName[54][7]=' ';enemyName[54][8]=' ';enemyName[54][9]=' ';enemyName[54][10]=' ';enemyName[54][11]='\0';
  enemyName[55][0]='H';enemyName[55][1]='a';enemyName[55][2]='u';enemyName[55][3]='s';enemyName[55][4]='m';enemyName[55][5]='e';enemyName[55][6]='i';enemyName[55][7]='s';enemyName[55][8]='t';enemyName[55][9]='e';enemyName[55][10]='r';enemyName[55][11]='\0';
  enemyName[56][0]='T';enemyName[56][1]='u';enemyName[56][2]='f';enemyName[56][3]='f';enemyName[56][4]=' ';enemyName[56][5]=' ';enemyName[56][6]=' ';enemyName[56][7]=' ';enemyName[56][8]=' ';enemyName[56][9]=' ';enemyName[56][10]=' ';enemyName[56][11]='\0';
  enemyName[57][0]='W';enemyName[57][1]='u';enemyName[57][2]='t';enemyName[57][3]='o';enemyName[57][4]='k';enemyName[57][5]=' ';enemyName[57][6]=' ';enemyName[57][7]=' ';enemyName[57][8]=' ';enemyName[57][9]=' ';enemyName[57][10]=' ';enemyName[57][11]='\0';
  enemyName[58][0]='T';enemyName[58][1]='u';enemyName[58][2]='f';enemyName[58][3]='f';enemyName[58][4]=' ';enemyName[58][5]=' ';enemyName[58][6]=' ';enemyName[58][7]=' ';enemyName[58][8]=' ';enemyName[58][9]=' ';enemyName[58][10]=' ';enemyName[58][11]='\0';
  enemyName[59][0]='W';enemyName[59][1]='u';enemyName[59][2]='t';enemyName[59][3]='o';enemyName[59][4]='k';enemyName[59][5]=' ';enemyName[59][6]=' ';enemyName[59][7]=' ';enemyName[59][8]=' ';enemyName[59][9]=' ';enemyName[59][10]=' ';enemyName[59][11]='\0';

  enemyName[60][0]='L';enemyName[60][1]='i';enemyName[60][2]='n';enemyName[60][3]='u';enemyName[60][4]='x';enemyName[60][5]=' ';enemyName[60][6]=' ';enemyName[60][7]=' ';enemyName[60][8]=' ';enemyName[60][9]=' ';enemyName[60][10]=' ';enemyName[60][11]='\0';
  enemyName[61][0]='H';enemyName[61][1]='a';enemyName[61][2]='u';enemyName[61][3]='s';enemyName[61][4]='m';enemyName[61][5]='e';enemyName[61][6]='i';enemyName[61][7]='s';enemyName[61][8]='t';enemyName[61][9]='e';enemyName[61][10]='r';enemyName[61][11]='\0';
  enemyName[62][0]='T';enemyName[62][1]='u';enemyName[62][2]='f';enemyName[62][3]='f';enemyName[62][4]=' ';enemyName[62][5]=' ';enemyName[62][6]=' ';enemyName[62][7]=' ';enemyName[62][8]=' ';enemyName[62][9]=' ';enemyName[62][10]=' ';enemyName[62][11]='\0';
  enemyName[63][0]='W';enemyName[63][1]='u';enemyName[63][2]='t';enemyName[63][3]='o';enemyName[63][4]='k';enemyName[63][5]=' ';enemyName[63][6]=' ';enemyName[63][7]=' ';enemyName[63][8]=' ';enemyName[63][9]=' ';enemyName[63][10]=' ';enemyName[63][11]='\0';
  enemyName[64][0]='W';enemyName[64][1]='u';enemyName[64][2]='t';enemyName[64][3]='o';enemyName[64][4]='k';enemyName[64][5]=' ';enemyName[64][6]=' ';enemyName[64][7]=' ';enemyName[64][8]=' ';enemyName[64][9]=' ';enemyName[64][10]=' ';enemyName[64][11]='\0';
 


  PlayerID=3;                   // laufende Nummer des Spielers
  Player=enemyName[PlayerID];   //programm hohlt sich den Spielernamen aus der enemyName liste ueber die PlayerID
  Team=3;           // moegliche TEAMS: 1, 2, 3, 4
  Damage=10;       // Folgende Werte sind erlaubt: 1,2,4,5,7,10,15,17,20,25,30,35,40,50,75,100
  Health=100;     // Lebenspunkte mit denen man spielt, max Wert 100 erlaubt
  Life=1;         // Anzahl der Leben mit denen man Spielt
  Ammo=30;          //Anzahl der Munition im Magazin
  Clips=10;          // Anzahl an Magazinen am Start
  ammoUse=2;       // Munitionsverbrauch pro SchuÃŸ
  respawnTime=1;    // Zeitdauer in Minuten nachdem man Respawnt(nicht mehr als 500 min. eingeben)
  fullAuto=true;   // true: Full Auto ein, false Einzelfeuer aktiv
  respawn=true;    // true: man kann nach 0 leben noch respawnen    false: geht nicht mehr
  friendlyFire= true;  // an: true, aus; false
  clipUse=true;       // Unednlich Magazine:false  Magazinverbrauch: true
  shortRange=true;      // true: lange reichweite wird verwendet, false: kurze Reichweite wird verwendet
  indoorMode=true;  //Indoor Reichweite: true  Outdoor Reichweite: false
  sound=true;     // true: keine Soundwiedergabe false: Soundwidergabe an

 /* Spielvariablen für den Spielmodus Capture the Flag(noch in Entwicklung)
  ctf=false;          // Capture the Flag an: true      aus: false
  flagFire=false;     // mit Flagge schiessen: true      nicht schiessen: false                 
 */ 


  pinMode(sendLed, OUTPUT);
  pinMode(sendLedIndoor, OUTPUT);
  pinMode(sendLedShort, OUTPUT);
  pinMode(sendLedShortIndoor, OUTPUT);
  
  pinMode(recvPin, INPUT);

  pinMode(triggerPin, INPUT);
  digitalWrite(triggerPin, HIGH);      //pullop Wiederstan einschalten
  pinMode(reloadPin, INPUT);
  digitalWrite(reloadPin, HIGH);       //pullop Wiederstand einschalten
  pinMode(gamePin, INPUT);
  digitalWrite(gamePin, HIGH);        //pullop Wiederstand einschalten

  pinMode(muzzleLed, OUTPUT);
  pinMode(hitLed, OUTPUT);

  pinMode(noSoundPin, OUTPUT);
  pinMode(lcdBackroundLight, OUTPUT);

  lcd.begin(16,2);


  playerBinary();    // berechnungsprogramm zur umrechnung der Player ID in BinÃ¤rcode
  ammo=Ammo;        // Anfangsmunition ist die Munitionsgroesse
  clips=Clips;      // Anfangsmagazine ist die Magazinmenge

  //Nur für Testzwecke Verändern
  Healed=false;
  Game=true;
  Pause=false;
  AdminKill=false;
  Flag=false;
  DataRecv=false;
  pressedReloadPin=false;
  outOfClips=false;
  gameOver=false;
  outOfAmmo=false;
  incSerial=false;
  hited=false;
  CtfLed=false;
  dead=false;
  hitLedOn=false;
  stateBusyPinHigh=false;
  digitalWrite(noSoundPin, LOW);
  digitalWrite(lcdBackroundLight, HIGH);
  
  //Für Testzwecke erstellte enemyHits
  
 /* for(int i=1; i<=64; i++)
  {
    enemyHits[i]=i*1000-(i*100);
    Serial.println(enemyHits[i]);
  }*/
}

void loop()
{
 
  recieveIR();        //startet unterprogramm zum Empfangen von IR-Signalen
  
  if(digitalRead(busyPin)==HIGH)
  {
    stateBusyPinHigh=true;
  }
  if(digitalRead(busyPin)==LOW&&hitLedOn==false)
  {
    digitalWrite(noSoundPin, LOW);
    stateBusyPinHigh=false;
  }
  if(Game==true&&AdminKill==false&&gameOver==false)
  {
    if(dead==true)
    {
      lcd.setCursor(0,0);
      lcd.print(F("You are Down"));
      digitalWrite(hitLed, HIGH);
    }
    if(dead==false&&pressedReloadPin==false&&outOfClips==false&&outOfAmmo==false)
    {
      lcd.setCursor(0,0);     //Setzt den Cursor auf Zeichen 1 in Reihe 1
      lcd.print(Player);      //schreibt den Wert Playername ab zeichen 1
      lcd.setCursor(14,0);    //Setzt den Cursor auf Zeichen 14 in Reihe 1
      lcd.print(F("T"));      //schreibt "T:" ab Zeichen 14
      lcd.setCursor(15,0);
      lcd.print(Team);
      lcd.setCursor(0, 1);
      lcd.print(F("H"));
      lcd.setCursor(1,1);
      lcd.print(Health);
      lcd.setCursor(5,1);
      lcd.print(F("L"));
      lcd.setCursor(6,1);
      lcd.print(Life);
      lcd.setCursor(9,1);
      lcd.print(F("A"));
      lcd.setCursor(10,1);
      lcd.print(ammo);
      lcd.setCursor(13,1);
      lcd.print(F("C"));
      lcd.setCursor(14,1);
      lcd.print(clips);
    }
    switchState();

    if( hited==true&&Health>>0)
    {
      if((currentMillis-850)>=millis())
      {
        digitalWrite(noSoundPin,HIGH);
      }
      if(currentMillis>=millis())
      {
        digitalWrite(hitLed, HIGH);   
      }
      else
      {
        digitalWrite(hitLed, LOW);
        hitLedOn=false;
        hited=false;     
      }
    }
//Die Folgenden Programmzeilen sind noch in entwicklung und dienen dem Spielmodus Capture the Flag
/*
    if(ctf==true&&Flag==true&&Pause==false)   //Wenn CTF aktiv dann startet das Unterprogramm dazu
    { 
      if(CtfLed==true)
      {
        digitalWrite(hitLed, HIGH);
        if(currentMillisCtf<=millis())
        {
          digitalWrite(hitLed,LOW);
          currentMillisCtf=millis()+1000;
          CtfLed=false;
        }
      }
      if(CtfLed==false)
      {
        digitalWrite(hitLed, LOW);
        if(currentMillisCtf<=millis())
        {
          currentMillisCtf=millis()+1000;
          CtfLed=true;
        }
      }
    }*/
  }

  if(Game==false&&gameOver==false)
  { 
    lcd.clear();
    gameOver=true;
  }

  if(gameOver==true)
  {
    if(pressedReloadPin==false&&pressedTriggerPin==false)
    {
    lcd.setCursor(0,0);
    lcd.print(F("Game Over Or Not"));
    lcd.setCursor(0,1);
    lcd.print(F("Yet Started"));
    }
  }
  
//Die Folgenden Programmzeilen sind noch in entwicklung und dienen der Auswertung via Serieller Schnittstelle
// oder per Infrarot
/*  if(gameOver==true)
  {
    if(digitalRead(reloadPin)==LOW&&pressedTriggerPin==false)
    {
      pressedReloadPin=true;
      lcd.clear();
    } 
    if(pressedReloadPin==true)
    {
      lcd.setCursor(0,0);
      lcd.print(F("Sending Score"));
      lcd.setCursor(0,1);
      lcd.print(F("Serial"));
      scoreSerial();
    }
    
   if(Serial.available()>0&&pressedReloadPin==false)   //fragt serielle daten ab
    {
      recieveSerial();
      playerBinary();
    }
    
    if(digitalRead(triggerPin)==LOW&&pressedReloadPin==false)
    {
      pressedTriggerPin=true;
      lcd.clear();
    }
    if(pressedTriggerPin==true)
    {
      lcd.setCursor(0,0);
      lcd.print(F("Sending Scores"));
      lcd.setCursor(0,1);
      lcd.print(F("Infrared"));
      
      if(indoorMode==false)
      {
       indoorMode=true;
       shoot();
       scoreShoot();
       pressedTriggerPin=false;
       indoorMode=false;
      }
      else
      {
       shoot();
       scoreShoot();
       pressedTriggerPin=false;
      }
    }
  }
 //fragt Serielle Daten ab um die Spielparameter zu verändern 
 if(Serial.available()>0 && pressedReloadPin==false)   //fragt serielle daten ab
    {
      recieveSerial();
    }*/

}   // Ende des Hauptloops


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////switchState////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void switchState()
{
 if(dead==false)
 {
  if(Pause==false && ctf ==false &&flagSend==false || Pause==false && ctf==true && Flag==false &&flagSend==false || Pause==false && ctf==true && Flag==true && flagFire==true &&flagSend==false )
  {
    if(digitalRead(triggerPin)==LOW && Health >0 && ammo>0 && pressedReloadPin==false && pressedTriggerPin==false && hited==false) //es kann nur geschoÃŸen werden wenn man am leben ist und munition hat
    { 
      if(stateBusyPinHigh==false)
      {
       if(ammoUse==3&&sound==true)
       {
        digitalWrite(noSoundPin, LOW);
        wtv020sd16p.asyncPlayVoice(3);
       }
      if(ammoUse==2&&sound==true)
      {
       digitalWrite(noSoundPin, LOW);
       wtv020sd16p.asyncPlayVoice(2);
      }
      if(ammoUse==1&&sound==true)
       {
        digitalWrite(noSoundPin, LOW);
        wtv020sd16p.asyncPlayVoice(1);
       }
      //digitalWrite(noSoundPin, HIGH);
      pressedTriggerPin=true;
      shoot();     // starte unterprogramm shoot
      ammo=ammo-ammoUse;
      shots=shots+1;
      
     if(hitLedOn==false)
     {
      for(int a=1; a<=ammoUse; a++)
      { 
        if(hitLedOn==false)
        {
         digitalWrite(muzzleLed, HIGH);
        }
         recieveIR2();  
         digitalWrite(muzzleLed, LOW);
         recieveIR2();
         if(sound==true)
         {
         digitalWrite(noSoundPin, HIGH);  
         }    
       }
     }
      if(fullAuto==true&&digitalRead(triggerPin)==LOW)
      {
        pressedTriggerPin=false;
      }
    }
     if(stateBusyPinHigh==true)
      { 
      if(ammoUse==3&&sound==true)
      {
       digitalWrite(noSoundPin, LOW);
       wtv020sd16p.asyncPlayVoice(3);   
      }
      if(ammoUse==2&&sound==true)
      {
      digitalWrite(noSoundPin, LOW);
       wtv020sd16p.asyncPlayVoice(2);  
      }
     if(ammoUse==1&&sound==true)
      {
       digitalWrite(noSoundPin, LOW);
       wtv020sd16p.asyncPlayVoice(1);   
      }
      
      pressedTriggerPin=true;
      shoot();     // starte unterprogramm shoot
      ammo=ammo-ammoUse;
      shots=shots+1;
      
     if(hitLedOn==false)
     { 
      for(int a=1; a<=ammoUse; a++)
      {
        if(hitLedOn==false)
        {
         digitalWrite(muzzleLed, HIGH);
        }
         recieveIR2();          
         digitalWrite(muzzleLed, LOW);
         recieveIR2();

        if(sound==true)
         { 
          digitalWrite(noSoundPin, HIGH);
         }
       }
     }
 
      if(fullAuto==true&&digitalRead(triggerPin)==LOW)
      {
        pressedTriggerPin=false;
      }
      }
      lcd.clear(); 
    }
    
    if(fullAuto==false&&digitalRead(triggerPin)==HIGH)
    {
      pressedTriggerPin=false;
    }
    if(ammo<=0&&Health>>0&&hited==false&&digitalRead(triggerPin)==LOW&& pressedTriggerPin==false)
    {
      pressedTriggerPin=true;
      if(stateBusyPinHigh==false&&sound==true)
      {
        digitalWrite(noSoundPin, LOW);
        wtv020sd16p.asyncPlayVoice(8);
        recieveIR();
        recieveIR();
        recieveIR();
        recieveIR();
        recieveIR();
        digitalWrite(noSoundPin, HIGH);
      }
      if(stateBusyPinHigh==true&&sound==true)
      {
        digitalWrite(noSoundPin, LOW);
        wtv020sd16p.asyncPlayVoice(8);
        recieveIR();
        recieveIR();
        recieveIR();
        recieveIR();
        digitalWrite(noSoundPin, HIGH);
      }   
    }    
   
    if(ammo<=0&&pressedTriggerPin==true&&outOfAmmo==false&&hited==false)
    { 
      currentMillis=millis()+3000;
      outOfAmmo=true;
      lcd.clear();  
    }
    if(digitalRead(triggerPin)==HIGH)
     {
      pressedTriggerPin=false;
     }
    
    if(outOfAmmo==true)
    {
      if(currentMillis>=millis())
      {
        lcd.setCursor(0,0);
        lcd.print(F("Out Of Ammo"));
      }
      else
      {
        lcd.clear();
        outOfAmmo=false;
      }   
    }

    if(digitalRead(reloadPin)==LOW && pressedReloadPin==false&& pressedTriggerPin==false && hited==false)
    {
      if(clipUse==true)
      {
      if(clips>=1)
      {
        pressedReloadPin=true;
        lcd.clear();
        lcd.setCursor(0,0);
      }
      else
      {
        outOfClips=true;
        lcd.clear();
        lcd.setCursor(0,0);
      }
    } 
    if(clipUse==false)
    {
      pressedReloadPin=true;
      lcd.clear();
      lcd.setCursor(0,0);
    }

    if(pressedReloadPin==true)
    { 
     if(stateBusyPinHigh==false)
     {
      if(sound==true)
      {
       digitalWrite(noSoundPin, LOW);
       wtv020sd16p.asyncPlayVoice(4);
      }
       lcd.setCursor(0,0);
       lcd.print(F("Reloading"));
       recieveIR2();
       recieveIR2();
       recieveIR2();
       recieveIR2();
       recieveIR2();
       
       if(sound==true)
      {
       digitalWrite(noSoundPin, HIGH);
      }
     }
     if(stateBusyPinHigh==true)
     {
      if(sound==true)
      {
       digitalWrite(noSoundPin, LOW);
       wtv020sd16p.asyncPlayVoice(4);
      }
       lcd.setCursor(0,0);
       lcd.print(F("Reloading"));
       recieveIR2();
       recieveIR2();
       recieveIR2();
       recieveIR2();
       
       if(sound==true)
      {
       digitalWrite(noSoundPin, HIGH);
      }
     }    
       if(hited==false)
       { 
        for(int i=0; i<=225; i++)
        {recieveIR2();}
       }
        lcd.clear();
        if(hited==false)
        {
         ammo=Ammo;
         if(clipUse==true)
         {
           clips=clips-1;
         }
         pressedReloadPin=false;
        }     
    }

    if(outOfClips==true&&hited==false)
    { 

      if(currentMillis>=millis())
      {
        lcd.setCursor(0,0);
        lcd.print(F("Out Of Clips"));
      }
      else
      {
        lcd.clear();
        pressedReloadPin=false;
        outOfClips=false;
      }
    }
  }
}

/*
 * Diese Programmzeilen dienen für den Spielmodus Capture the Flag welcher noch in entwicklung ist
if(digitalRead(triggerPin)==LOW&&ctf==true&&Flag==true||digitalRead(reloadPin)==LOW&&ctf==true&&Flag==true)
  {
    flagSend=true;
    currentMillis=millis()+100;
    
    if(flagSend==true)
    { 
      if(currentMillis<<millis())
      { 
        if(digitalRead(triggerPin)==LOW)
        {
          if(digitalRead(reloadPin)==LOW)
          {
            ctfGame();
            flagSend=false;
          }
          digitalWrite(hitLed, LOW);
          CtfLed=false;
          flagSend=false;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(F("HALLO"));
          delay(5000);
          lcd.clear();
        }
      }  
    }
  }*/
 } //ende der IF(dead==false) schleife
}



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////Unterprogramm Shoot////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void shoot()
{
  if(indoorMode==true&&shortRange==false)  //Falls Indoormodus aktiv ist und die lange Reichweite verwendet wird
  {
   sendPulseIndoor(startBit, divisorstart);   //in der() gibt an welche zeit und der divisor dient zum ausgleich der abweichungen
   for(int i=0; i<=7; i++)
   {
     if (bite1[i] == 0)       
     {
       sendPulseIndoor(zeroBit, divisorzero);
       //Serial.println("bit war 0");
     }
     else
     {
       sendPulseIndoor(oneBit,divisorone);
       //Serial.println("bit war 1");
     }
   }

  for(int i=0; i<6; i++)
  {
    if (bite2[i] == 0)
    {
      sendPulseIndoor(zeroBit, divisorzero);
      //Serial.println("bit war 0");
    }
    else
    {
      sendPulseIndoor(oneBit, divisorone);
      //Serial.println("bit war 1");
    }
  }
 }
  
   if(indoorMode==true&&shortRange==true)  //Falls Indoormodus und kurze Reichweite aktiv ist wird die kurze Indoor Reichweite verwendet
  {
   sendPulseShortIndoor(startBit, divisorstart);   //in der() gibt an welche zeit und der divisor dient zum ausgleich der abweichungen
   for(int i=0; i<=7; i++)
   {
     if (bite1[i] == 0)       
     {
       sendPulseShortIndoor(zeroBit, divisorzero);
       //Serial.println("bit war 0");
     }
     else
     {
       sendPulseShortIndoor(oneBit,divisorone);
       //Serial.println("bit war 1");
     }
   }
 
  for(int i=0; i<6; i++)
  {
    if (bite2[i] == 0)
    {
      sendPulseShortIndoor(zeroBit, divisorzero);
      //Serial.println("bit war 0");
    }
    else
    {
      sendPulseShortIndoor(oneBit, divisorone);
      //Serial.println("bit war 1");
    }
  }
 }
  
   if(indoorMode==false&&shortRange==false)//Falls die lange Reichweite Outdoor verwendet wird
  {
   sendPulse(startBit, divisorstart);   //in der() gibt an welche zeit und der divisor dient zum ausgleich der abweichungen
   for(int i=0; i<=7; i++)
   {
     if (bite1[i] == 0)       
     {
       sendPulse(zeroBit, divisorzero);
       //Serial.println("bit war 0");

     }
     else
     {
       sendPulse(oneBit,divisorone);
       //Serial.println("bit war 1");
     }
   }
   for(int i=0; i<6; i++)
  {
    if (bite2[i] == 0)
    {
      sendPulse(zeroBit, divisorzero);
      //Serial.println("bit war 0");
    }
    else
    {
      sendPulse(oneBit, divisorone);
      //Serial.println("bit war 1");
    }
  }
  }
   if(indoorMode==false&&shortRange==true)//Falls die kurze Reichweite Outdoor verwendet wird
  {
   sendPulseShort(startBit, divisorstart);   //in der() gibt an welche zeit und der divisor dient zum ausgleich der abweichungen
   for(int i=0; i<=7; i++)
   {
     if (bite1[i] == 0)       
     {
       sendPulseShort(zeroBit, divisorzero);
       //Serial.println("bit war 0");

     }
     else
     {
       sendPulseShort(oneBit,divisorone);
       //Serial.println("bit war 1");
     }
   }
   for(int i=0; i<6; i++)
  {
    if (bite2[i] == 0)
    {
      sendPulseShort(zeroBit, divisorzero);
      //Serial.println("bit war 0");
    }
    else
    {
      sendPulseShort(oneBit, divisorone);
      //Serial.println("bit war 1");
    }
  }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Unterprogramm zum Senden der Pulse Indoor/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendPulseIndoor(int time, int divisor)    //Unterprogramm in dem die SignallÃ¤nge des Bits fÃ¼r shoot berechnet wird
{
  //Serial.println(time);
  for(int i=0; i<= time/divisor; i++)
  {
    digitalWrite(sendLedIndoor, HIGH);
    delayMicroseconds(13);
    digitalWrite(sendLedIndoor, LOW);
    delayMicroseconds(13);
  }
  delayMicroseconds(600);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Unterprogramm zum Senden der Pulse Indoor Short/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendPulseShortIndoor(int time, int divisor)    //Unterprogramm in dem die SignallÃ¤nge des Bits fÃ¼r shoot berechnet wird
{
  //Serial.println(time);
  for(int i=0; i<= time/divisor; i++)
  {
    digitalWrite(sendLedShortIndoor, HIGH);
    delayMicroseconds(13);
    digitalWrite(sendLedShortIndoor, LOW);
    delayMicroseconds(13);
  }
  delayMicroseconds(600);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Unterprogramm zum Senden der Pulse Outdoor//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendPulse(int time, int divisor)    //Unterprogramm in dem die SignallÃ¤nge des Bits fÃ¼r shoot berechnet wird
{
  Serial.println("schuss");
  for(int i=0; i<= time/divisor; i++)
  {
    digitalWrite(sendLed, HIGH);
    delayMicroseconds(13);
    digitalWrite(sendLed, LOW);
    delayMicroseconds(13);
  }
  delayMicroseconds(600);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Unterprogramm zum Senden der Pulse Short Outdoor//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sendPulseShort(int time, int divisor)    //Unterprogramm in dem die SignallÃ¤nge des Bits fÃ¼r shoot berechnet wird
{
  //Serial.println(time);
  for(int i=0; i<= time/divisor; i++)
  {
    digitalWrite(sendLedShort, HIGH);
    delayMicroseconds(13);
    digitalWrite(sendLedShort, LOW);
    delayMicroseconds(13);
  }
  delayMicroseconds(600);
}
/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////recieveIR///////////////////////////////////////////////////////
//////////Unterprogramm um IR-Daten zu erhalten/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void recieveIR()
{
  int zeropulse;
  boolean shootpakage;
  boolean datapakage;
  int incDamage;
  int enemyTeam;
  int enemy=0;
  int DataType=0;
  int DataValue=0;
  int DataEnd=0;


  if(shootpakage==false && datapakage==false);
  {
    int pulse = pulseIn(recvPin, LOW, 150000);
   Serial.println("PULSE=");
    Serial.println(pulse);

    if(pulse <=2700 && pulse >=2100)   //fragt das startBit ab
    {
      zeropulse= pulseIn(recvPin, LOW,2200);

      if(zeropulse>=300 && zeropulse <=900 && Health>>0&&Pause==false) //fragt das Shootpaket Ã¼ber den Nullbit am anfang ab
      {      
        recieved1[7]=zeropulse;    
        for(int i=6; i>=0; i--)
        {
          recieved1[i] = pulseIn(recvPin, LOW,2200);
        }
        for(int i=5; i>=0; i--)
        {
          recieved2[i] = pulseIn(recvPin, LOW,2200);
        }
        //Serial.println("zeropulse");
        Serial.println(recieved1[7]);
        for(int i=6; i>=0; i--)
        {
         Serial.println(recieved1[i]);
        }
        for(int i=5; i>=0; i--)
        {
          Serial.println(recieved2[i]);
        }
        shootpakage = true;
        datapakage = false;
      }
      if(zeropulse>=1000 && zeropulse<=1500) //fragt datenpaket Ã¼ber eine 1 als Bit ab
      {
        recievedData1[7] = zeropulse;
        for(int i=6; i>=0; i--)
        {
          recievedData1[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recievedData1[i]);
        }
        for(int i=7; i>=0; i--)
        {
          recievedData2[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recievedData2[i]);  
        }
        for(int i=7; i>=0; i--)
        {
          recievedData3[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recievedData3[i]);         
          shootpakage = false; 
          datapakage = true; 
        }
        for(int i=6; i>=0; i--)
      {
        Serial.println(recievedData1[i]);
      }
       for(int i=7; i>=0; i--)
      {
        Serial.println(recievedData2[i]);
      } 
      for(int i=7; i>=0; i--)
      {
        Serial.println(recievedData3[i]);
      }
      } 
      
      if(shootpakage == true && Health >>0) //Umrechnung des Schusspaketes erfolgt nur wenn man noch Leben besitzt
      {       
        Serial.println("Umrechnung in Binaer erfolgt");
        for(int i=7; i>=0; i--)
        {
          if(recieved1[i] >=300 && recieved1[i] <=1500)
          {
            if(recieved1[i] >=1000 && recieved1[i] <=1500)
            { 
              recieved1[i]=1;
            }
            if(recieved1[i] >=300 && recieved1[i] <=900)
            { 
              recieved1[i]=0;
            }
            Serial.println(recieved1[i]);
          }
          else
          {        
            Serial.println("Eingangsbit(1) war ausserhalb der norm");
             recieved1[0]=2;
            recieved1[1]=2;
            recieved1[2]=2;
            recieved1[3]=2;
            recieved1[4]=2;
            recieved1[5]=2;
            recieved1[6]=2;
            recieved1[7]=2;
          }
        }
        Serial.println("Byte 1 fertig");

        for(int i=5; i>=0; i--)
        {
          if(recieved2[i] >=300 && recieved1[i] <=1500)
          {
            if(recieved2[i] >=1000 && recieved1[i] <=1500)
            { 
              recieved2[i]=1;
            }
            if(recieved2[i] >=300 && recieved1[i] <=900)
            { 
              recieved2[i]=0;
            }
            Serial.println(recieved2[i]);
          }
          else
          {
            recieved2[0]=2;
            recieved2[1]=2;
            recieved2[2]=2;
            recieved2[3]=2;
            recieved2[4]=2;
            recieved2[5]=2;          
            Serial.println("Eingangsbit(2) war auÃŸerhalb der norm");
          }
        }
        Serial.println("Byte 2 fertig");
        int dec;
           
        for(int i=0; i<=6; i++) //umrechnung in Dezimal erfolgt
        {
          if(recieved1[i]<=1)
          {
           dec= recieved1[i]*(1<<i);
           enemy= enemy+dec;  
          }
          else
          {
            recieved1[0]=2;
            recieved1[1]=2;
            recieved1[2]=2;
            recieved1[3]=2;
            recieved1[4]=2;
            recieved1[5]=2;
            recieved1[6]=2;
            enemy=0;
            
          }
        }         
        Serial.println("EnemyID:");
        Serial.println(enemy);

          if(recieved2[5]<=1&&recieved2[4]<=1)
     {
        if(recieved2[5]==1&&recieved2[4]==1)
      {
       enemyTeam=4;
      }
      if(recieved2[5]==1&&recieved2[4]==0)
      {
       enemyTeam=3;
      }
         if(recieved2[5]==0&&recieved2[4]==1)
      {
       enemyTeam=2;
      }
      if(recieved2[5]==0&&recieved2[4]==0)
      {
       enemyTeam=1;
      } 
      }
      else
      { 
        enemyTeam=0;
      }
        Serial.println(enemyTeam);

        //jetzt folgt eine sehr lang Programmzeile zum Umwandeln des Binaercodes in Schaden(DEC)
     if(recieved2[3]<=1&&recieved2[2]<=1&&recieved2[1]<=1&&recieved2[0]<=1)
     {
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 100;
        }
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 75;
        }
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 50;
        }
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 40;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 35;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 30;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 25;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 20;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 17;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 15;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 10;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 7;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 5;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 4;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 2;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 1;
        }
        Serial.println(incDamage);
     }
     else
     {
       recieved2[3]=2;
       recieved2[2]=2;
       recieved2[1]=2;
       recieved2[0]=2;
       incDamage=0;

     }
        if(enemyTeam==Team && friendlyFire== false || enemy==PlayerID || enemy==0 || incDamage==0 || enemyTeam==0 )  // fragt den Freundbeschuss ab bzw. ob mann sich selbst getroffen hat
        {
          for(int i=0; i<=6; i++)
          {
            recieved1[i]=2;
          }
          for(int i=0; i<=5; i++)
          {
            recieved2[i]=2;
          }
        }
        else
        {
          hited=true;
           enemyHits[enemy]=enemyHits[enemy]+incDamage; // Speichert den Schaden des Gegners ab den man erhalten hat
           Health=Health-incDamage;   // berechnung der restlichen Gesundheit
           lcd.clear();
           digitalWrite(hitLed, HIGH);
           
           if(hitLedOn==false)
           {
            currentMillis=millis()+1000;
            if(sound==true)
            {
            digitalWrite(noSoundPin, LOW);
            wtv020sd16p.asyncPlayVoice(5);
            recieveIR2();
            digitalWrite(noSoundPin, HIGH);
            }
           }
           
        if(hitLedOn==false)
          {
           hitLedOn=true;
           digitalWrite(muzzleLed,LOW);
          }   
        }

        if(Health<=0 && Life>=1)//Es wird abgefragt ob mann noch leben hat, falls mann tod ist wird darauf reagiert
        {
          if(sound==true)
          {
          digitalWrite(noSoundPin, LOW);
          wtv020sd16p.asyncPlayVoice(6);
          delay(125);//delay anpassen!!!!!!!!!!!!
          digitalWrite(noSoundPin, HIGH);
          }
          lcd.clear();
          digitalWrite(hitLed, HIGH);
          Flag=false;
          lcd.setCursor(0,0);
          lcd.print(F("Killed by:"));
          lcd.setCursor(0,1);
          lcd.print(enemyName[enemy]);
          lcd.setCursor(13,1);
          lcd.print(F("T:"));
          lcd.setCursor(15,1);
          lcd.print(enemyTeam);
          delay(5000);
          lcd.clear();
          /*lcd.setCursor(0,0);
          lcd.print("Wait for:");
          lcd.setCursor(0,1);*/
          
          for(int i=(respawnTime*60); i>=2; i--)
          {
            if(i>=10000)
            {
             lcd.setCursor(0,0);
             lcd.print(F("Wait for:"));
             lcd.setCursor(0,1);
             lcd.print(i);
             lcd.setCursor(6,1);
             lcd.print(F("sec."));
             delay(1000);
             lcd.clear();
            }
            if(i>=1000&&i<10000)
            {
             lcd.setCursor(0,0);
             lcd.print(F("Wait for:"));
             lcd.setCursor(0,1);
             lcd.print(i);
             lcd.setCursor(5,1);
             lcd.print(F("sec."));
             delay(1000);
             lcd.clear();
            }
            if(i>=100&&i<1000)
            {
             lcd.setCursor(0,0);
             lcd.print(F("Wait for:"));
             lcd.setCursor(0,1);
             lcd.print(i);
             lcd.setCursor(4,1);
             lcd.print(F("sec."));
             delay(1000);
             lcd.clear();
            }
            if(i>=10&&i<100)
            {
             lcd.setCursor(0,0);
             lcd.print(F("Wait for:"));
             lcd.setCursor(0,1);
             lcd.print(i);
             lcd.setCursor(3,1);
             lcd.print(F("sec."));
             delay(1000);
             lcd.clear();
            }
            if(i<=9&&i>=3&&i<10)
            {
             lcd.setCursor(0,0);
             lcd.print(F("Wait for:"));
             lcd.setCursor(0,1);
             lcd.print(i);
             lcd.setCursor(2,1);
             lcd.print(F("sec."));
             delay(1000);
             lcd.clear();
            }
            
            if(i==2)
            {
              if(sound==true)
              {
              digitalWrite(noSoundPin, LOW);
              wtv020sd16p.asyncPlayVoice(7);
              }
              lcd.setCursor(0,0);
              lcd.print(F("Wait for:"));
              lcd.setCursor(0,1);
              lcd.print(F("2 sec."));
              delay(100);
              if(sound==true)
              {
              digitalWrite(noSoundPin, HIGH);
              }
              delay(900);
              lcd.setCursor(0,1);
              lcd.print(F("1 sec."));
              delay(1000);
              lcd.clear();
              Health=100;
              Life=Life-1;
              Healed=false;
            }
          }
          
        }
        if(Health<=0 && Life<=0)// Schreibt bei einem kompletten Game Over wer einen eliminiert hat
        {
          if(sound==true)
          {
          digitalWrite(noSoundPin, LOW);
          wtv020sd16p.playVoice(6);
          }
          lcd.clear();
          digitalWrite(hitLed, HIGH);
          Flag=false;
          lcd.setCursor(0,0);
          lcd.print(F("Killed by:"));
          lcd.setCursor(0,1);
          lcd.print(enemyName[enemy]);
          lcd.setCursor(13,1);
          lcd.print(F("T:"));
          lcd.setCursor(15,1);
          lcd.print(enemyTeam);
          if(sound==true)
          { 
          delay(125);
          digitalWrite(noSoundPin, HIGH);
          }
          delay(5000);
          dead=true;
          lcd.clear();
          Healed=false;
        }
        shootpakage=false;
        
        
      }//Ende der Shootpaketumwandlung

      if(datapakage==true)// start der Umwandlung des Datenpaketes in Binaer Code
      {
        Serial.println("Umrechnung Data in Binaer erfolgt");
        for(int i=7; i>=0; i--)
        {
          if(recievedData1[i] >=400 && recievedData1[i] <=1400)
          {
            if(recievedData1[i] >=1000 && recievedData1[i] <=1400)
            { 
              recievedData1[i]=1;
            }
            if(recievedData1[i] >=400 && recievedData1[i] <=800)
            { 
              recievedData1[i]=0;
            }
            Serial.println(recievedData1[i]);
          }

          else
          {
            Serial.println("Eingangsbit(1) Data war auÃŸerhalb der norm");
            break;
          }
        }
        Serial.println("Databyte 1 fertig");

        for(int i=7; i>=0; i--)//Databyte 2 wird in Binear umgewandelt
        {
          if(recievedData2[i] >=400 && recievedData2[i] <=1400)
          {
            if(recievedData2[i] >=1000 && recievedData2[i] <=1400)
            { 
              recievedData2[i]=1;
            }
            if(recievedData2[i] >=400 && recievedData2[i] <=800)
            { 
              recievedData2[i]=0;
            }
            Serial.println(recievedData2[i]);
          }

          else
          {
            Serial.println("Eingangsbit(2) war auÃŸerhalb der norm");
            break;
          }
        }
        Serial.println("Databyte 2 fertig");

        for(int i=7; i>=0; i--)//Databyte 3 wird in Binear umgewandelt
        {
          if(recievedData3[i] >=400 && recievedData3[i] <=1400)
          {
            if(recievedData3[i] >=1000 && recievedData3[i] <=1400)
            { 
              recievedData3[i]=1;
            }
            if(recievedData3[i] >=400 && recievedData3[i] <=800)
            { 
              recievedData3[i]=0;
            }
            Serial.println(recievedData3[i]);
          }

          else
          {
            Serial.println("Eingangsbit(3) war auÃŸerhalb der norm");
            break;
          }
        }
        Serial.println("Databyte 3 fertig");  //Ende der BinÃ¤rumwandlung

        int dec;

        for(int i=0; i<=7; i++) //umrechnung Databyte1 in Dezimal erfolgt
        {
          dec= recievedData1[i]*(1<<i);
          DataType= DataType+dec;
        }
        
        Serial.println("Datatype:");
        Serial.println(DataType);

        for(int i=0; i<=7; i++) //umrechnung Databyte2 in Dezimal erfolgt
        {
          dec= recievedData2[i]*(1<<i);
          DataValue= DataValue+dec;  
        }
        
        Serial.println("DataValue:");
        Serial.println(DataValue);

        for(int i=0; i<=7; i++) //umrechnung Databyte3 in Dezimal erfolgt
        {
          dec= recievedData3[i]*(1<<i);
          DataEnd= DataEnd+dec;  
        }
        
        Serial.println("DataEnd:");
        Serial.println(DataEnd);

        if(DataEnd== 232)//Ueberprueft ob das DataEnd auch stimmt und somit die Daten richtig Ã¼bertragen wurden
        {
          if(DataType== 131)//Ueberprueft Datatype 
          {
            if(DataValue==0)
            {
               if(sound==true)
               {
               wtv020sd16p.playVoice(6);
               }
              AdminKill=true;
              Health=0;
              Life=0;
              Flag=false;
            }
            if(DataValue==1)
            {
              Serial.println("DATA1");
              Pause=true;
            }
            if(DataValue==1&&Pause==true)
            {
              Serial.println("DATA11");
              Pause=false;
            }
            if(DataValue==2)
            {
               if(sound==true)
              {
              wtv020sd16p.asyncPlayVoice(6);
              }
              Serial.println(DataValue);
              Serial.println("DATA2");
              Game=true;
             dead=false; 
              gameOver=false;
              lcd.clear();
            }
            if(DataValue==4&&AdminKill==false&&respawn==true)//Respawn
            {
               if(sound==true)
                {
                 wtv020sd16p.asyncPlayVoice(7);
                 }
              Health=100;
              dead=false;
              digitalWrite(hitLed, LOW);
            }
            if(DataValue==6);
            {
              clips=Clips;
            }
            if(DataValue==7&& DataValue!=2)
            {
              Serial.println(DataValue);
              Serial.println("DATA7");
              Game=false;
              gameOver=false;
              Serial.println(DataValue);
            }
            if(DataValue==13&&AdminKill==false)
            {
              Serial.println("Data13");
              wtv020sd16p.playVoice(8);
              Health=100;
              Healed=true;
            }
            if(DataValue==9&&AdminKill==false)
            {
              DataRecv==true;
            }
          }
          if(DataType==140 && ctf==true)  // CTF Ãœbersetzung
          { 
            if(DataValue>=0 && DataValue<=14)
            {
              Flag=true;
              flagNumber=DataValue;
              currentMillisCtf=millis()+1000;
            }
          }  

          datapakage=false;   
        }//Ende der Datenpaket Umwandlung
      }  
    }
  }  
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////recieveIR2///////////////////////////////////////////////////////
//////////Unterprogramm um IR-Daten zu erhalten/////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void recieveIR2()
{
  int zeropulse;
  boolean shootpakage;
  boolean datapakage;
  int incDamage;
  int enemyTeam;
  int enemy=0;
  int DataType=0;
  int DataValue=0;
  int DataEnd=0;


  if(shootpakage==false && datapakage==false);
  {
    int pulse = pulseIn(recvPin, LOW, 45000);
   // Serial.println("PULSE=");
    //Serial.println(pulse);

    if(pulse <=2700 && pulse >=2100)   //fragt das startBit ab
    {
      zeropulse= pulseIn(recvPin, LOW,2200);
      //Serial.println(zeropulse);

      if(zeropulse>=300 && zeropulse <=900 && Health>>0&&Pause==false) //fragt das Shootpaket Ã¼ber den Nullbit am anfang ab
      {  
        recieved1[7]=zeropulse;    
        for(int i=6; i>=0; i--)
        {
          recieved1[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recieved1[i]);
        }
        for(int i=5; i>=0; i--)
        {
          recieved2[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recieved2[i]);
        }
        //Serial.println("zeropulse");
        Serial.println(recieved1[7]);
        for(int i=6; i>=0; i--)
        {
         Serial.println(recieved1[i]);
        }
        for(int i=5; i>=0; i--)
        {
          Serial.println(recieved2[i]);
        }
        shootpakage = true;
        datapakage = false;
      }
      if(zeropulse>=1000 && zeropulse<=1500) //fragt datenpaket Ã¼ber eine 1 als Bit ab
      {
        recievedData1[7] = zeropulse;
        for(int i=6; i>=0; i--)
        {
          recievedData1[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recievedData1[i]);
        }
        for(int i=7; i>=0; i--)
        {
          recievedData2[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recievedData2[i]);  
        }
        for(int i=7; i>=0; i--)
        {
          recievedData3[i] = pulseIn(recvPin, LOW,2200);
          //Serial.println(recievedData3[i]);
          
          shootpakage = false; 
          datapakage = true; 
        }
        for(int i=6; i>=0; i--)
      {
        Serial.println(recievedData1[i]);
      }
       for(int i=7; i>=0; i--)
      {
        Serial.println(recievedData2[i]);
      } 
      for(int i=7; i>=0; i--)
      {
        Serial.println(recievedData3[i]);
      }
      } 
      

      if(shootpakage == true && Health >>0) //Umrechnung des SchuÃŸpaketes erfolgt nur wenn man noch Leben besitzt
      {
        
        //currentMillis=millis()+1000;
        Serial.println("Umrechnung in Binaer erfolgt");
        for(int i=7; i>=0; i--)
        {
          if(recieved1[i] >=300 && recieved1[i] <=1500)
          {
            if(recieved1[i] >=1000 && recieved1[i] <=1500)
            { 
              recieved1[i]=1;
            }
            if(recieved1[i] >=300 && recieved1[i] <=900)
            { 
              recieved1[i]=0;
            }
            Serial.println(recieved1[i]);
          }

          else
          {
            
            Serial.println("Eingangsbit(1) war ausserhalb der norm");
          }
        }
        Serial.println("Byte 1 fertig");

        for(int i=5; i>=0; i--)
        {
          if(recieved2[i] >=300 && recieved1[i] <=1500)
          {
            if(recieved2[i] >=1000 && recieved1[i] <=1500)
            { 
              recieved2[i]=1;
            }
            if(recieved2[i] >=300 && recieved1[i] <=900)
            { 
              recieved2[i]=0;
            }
            Serial.println(recieved2[i]);
          }
          else
          {
            recieved2[0]=2;
            recieved2[1]=2;
            recieved2[2]=2;
            recieved2[3]=2;
            recieved2[4]=2;
            recieved2[5]=2;
            
            Serial.println("Eingangsbit(2) war auÃŸerhalb der norm");
          }
        }
        Serial.println("Byte 2 fertig");
        int dec;
        
        
        for(int i=0; i<=6; i++) //umrechnung in Dezimal erfolgt
        {
          if(recieved1[i]<=1)
          {
           dec= recieved1[i]*(1<<i);
           enemy= enemy+dec;  
          }
          else
          {
            recieved1[0]=2;
            recieved1[1]=2;
            recieved1[2]=2;
            recieved1[3]=2;
            recieved1[4]=2;
            recieved1[5]=2;
            recieved1[6]=2;
            enemy=0;
            
          }
        }   
        
        Serial.println("EnemyID:");
        Serial.println(enemy);


      if(recieved2[5]<=1&&recieved2[4]<=1)
     {
        if(recieved2[5]==1&&recieved2[4]==1)
      {
       enemyTeam=4;
      }
      if(recieved2[5]==1&&recieved2[4]==0)
      {
       enemyTeam=3;
      }
         if(recieved2[5]==0&&recieved2[4]==1)
      {
       enemyTeam=2;
      }
      if(recieved2[5]==0&&recieved2[4]==0)
      {
       enemyTeam=1;
      } 
      }
      else
      { 
        enemyTeam=0;
      }
        Serial.println(enemyTeam);

        //jetzt folgt eine Arschlange Zeile zum Umwandeln des BinÃ¤rcodes in Schaden(DEC)
     if(recieved2[3]<=1&&recieved2[2]<=1&&recieved2[1]<=1&&recieved2[0]<=1)
     {
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 100;
        }
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 75;
        }
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 50;
        }
        if(recieved2[3]==1&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 40;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 35;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 30;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 25;
        }
        if(recieved2[3]==1&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 20;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 17;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 15;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 10;
        }
        if(recieved2[3]==0&&recieved2[2]==1&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 7;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==1)
        {
          incDamage= 5;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==1&&recieved2[0]==0)
        {
          incDamage= 4;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==1)
        {
          incDamage= 2;
        }
        if(recieved2[3]==0&&recieved2[2]==0&&recieved2[1]==0&&recieved2[0]==0)
        {
          incDamage= 1;
        }
        Serial.println(incDamage);
     }
     else
     {
       recieved2[3]=2;
       recieved2[2]=2;
       recieved2[1]=2;
       recieved2[0]=2;
       incDamage=0;

     }
        if(enemyTeam==Team && friendlyFire== false || enemy==PlayerID || enemy==0 || incDamage==0|| enemyTeam==0 )  // fragt den Freundbeschuss ab bzw. ob mann sich selbst getroffen hat
        {
          for(int i=0; i<=6; i++)
          {
            recieved1[i]=2;
          }
          for(int i=0; i<=5; i++)
          {
            recieved2[i]=2;
          }
        }
        else
        {
          hited=true;
           enemyHits[enemy]=enemyHits[enemy]+incDamage; // Speichert den Schaden des Gegners ab den man erhalten hat
           Health=Health-incDamage;   // berechnung der restlichen Gesundheit       
        }

        shootpakage=false;
        
        
      }//Ende der Shootpaketumwandlung

      if(datapakage==true)// start der Umwandlung des Datenpaketes in Binaer Code
      {
        Serial.println("Umrechnung Data in Binaer erfolgt");
        for(int i=7; i>=0; i--)
        {
          if(recievedData1[i] >=400 && recievedData1[i] <=1400)
          {
            if(recievedData1[i] >=1000 && recievedData1[i] <=1400)
            { 
              recievedData1[i]=1;
            }
            if(recievedData1[i] >=400 && recievedData1[i] <=800)
            { 
              recievedData1[i]=0;
            }
            Serial.println(recievedData1[i]);
          }

          else
          {
            Serial.println("Eingangsbit(1) Data war auÃŸerhalb der norm");
            break;
          }
        }
        Serial.println("Databyte 1 fertig");

        for(int i=7; i>=0; i--)//Databyte 2 wird in Binear umgewandelt
        {
          if(recievedData2[i] >=400 && recievedData2[i] <=1400)
          {
            if(recievedData2[i] >=1000 && recievedData2[i] <=1400)
            { 
              recievedData2[i]=1;
            }
            if(recievedData2[i] >=400 && recievedData2[i] <=800)
            { 
              recievedData2[i]=0;
            }
            Serial.println(recievedData2[i]);
          }

          else
          {
            Serial.println("Eingangsbit(2) war auÃŸerhalb der norm");
            break;
          }
        }
        Serial.println("Databyte 2 fertig");

        for(int i=7; i>=0; i--)//Databyte 3 wird in Binear umgewandelt
        {
          if(recievedData3[i] >=400 && recievedData3[i] <=1400)
          {
            if(recievedData3[i] >=1000 && recievedData3[i] <=1400)
            { 
              recievedData3[i]=1;
            }
            if(recievedData3[i] >=400 && recievedData3[i] <=800)
            { 
              recievedData3[i]=0;
            }
            Serial.println(recievedData3[i]);
          }

          else
          {
            Serial.println("Eingangsbit(3) war auÃŸerhalb der norm");
            break;
          }
        }
        Serial.println("Databyte 3 fertig");  //Ende der BinÃ¤rumwandlung

        int dec;

        for(int i=0; i<=7; i++) //umrechnung Databyte1 in Dezimal erfolgt
        {
          dec= recievedData1[i]*(1<<i);
          DataType= DataType+dec;
        }
        
        Serial.println("Datatype:");
        Serial.println(DataType);

        for(int i=0; i<=7; i++) //umrechnung Databyte2 in Dezimal erfolgt
        {
          dec= recievedData2[i]*(1<<i);
          DataValue= DataValue+dec;  
        }
        
        Serial.println("DataValue:");
        Serial.println(DataValue);

        for(int i=0; i<=7; i++) //umrechnung Databyte3 in Dezimal erfolgt
        {
          dec= recievedData3[i]*(1<<i);
          DataEnd= DataEnd+dec;  
        }
        
        Serial.println("DataEnd:");
        Serial.println(DataEnd);

       
        if(DataEnd== 232)//Ueberprueft ob das DataEnd auch stimmt und somit die Daten richtig Ã¼bertragen wurden
        {
          if(DataType== 131)//Ueberprueft Datatype 
          {
            if(DataValue==0)
            {
               if(sound==true)
               {
               wtv020sd16p.playVoice(6);
               }
              AdminKill=true;
              Health=0;
              Life=0;
              Flag=false;
            }
            if(DataValue==1)
            {
              Serial.println("DATA1");
              Pause=true;
            }
            if(DataValue==1&&Pause==true)
            {
              Serial.println("DATA11");
              Pause=false;
            }
            if(DataValue==2)
            {
               if(sound==true)
              {
              wtv020sd16p.asyncPlayVoice(6);
              }
              Serial.println(DataValue);
              Serial.println("DATA2");
              Game=true;
             dead=false; 
              gameOver=false;
              lcd.clear();
            }
            if(DataValue==4&&AdminKill==false&&respawn==true)//Respawn
            {
               if(sound==true)
                {
                 wtv020sd16p.asyncPlayVoice(7);
                 }
              Health=100;
              dead=false;
              digitalWrite(hitLed, LOW);
            }
            if(DataValue==6);
            {
              clips=Clips;
            }
            if(DataValue==7&& DataValue!=2)
            {
              Serial.println(DataValue);
              Serial.println("DATA7");
              Game=false;
              gameOver=false;
              Serial.println(DataValue);
            }
            if(DataValue==13&&AdminKill==false)
            {
              Serial.println("Data13");
              wtv020sd16p.playVoice(8);
              Health=100;
              Healed=true;
            }
            if(DataValue==9&&AdminKill==false)
            {
              DataRecv==true;
            }
          }
          if(DataType==140 && ctf==true)  // CTF Ãœbersetzung
          { 
            if(DataValue>=0 && DataValue<=14)
            {
              Flag=true;
              flagNumber=DataValue;
              currentMillisCtf=millis()+1000;
            }
          }  
          datapakage=false;   
        }
      }  
    }
  }  
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////Unterprogramm zur Umwandlung des Spielercodes/////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void playerBinary()   //Wandelt Teamnummer und Spielernummer in einen BinÃ¤rcode um
{

  bite1[0]= 0;
  int weiter = Team-1;
  int weiter2=PlayerID;
  
  for(int i=0; i<7; i++)    //Spielernummer wird umgewandelt
  {
    bite1[7-i] = weiter2%2;
    weiter2 = weiter2/2; 
  }  

  for(int i=1; i>=0; i--) //Teamnummer wird umgewandelt
  {
    bite2[i]=weiter%2;
    weiter=weiter/2;
  }

  if(Damage==1)
  {
    bite2[2]=0;
    bite2[3]=0;
    bite2[4]=0;
    bite2[5]=0;
  }
  if(Damage==2)
  {
    bite2[2]=0;
    bite2[3]=0;
    bite2[4]=0;
    bite2[5]=1;
  }
  if(Damage==4)
  {
    bite2[2]=0;
    bite2[3]=0;
    bite2[4]=1;
    bite2[5]=0;
  }
  if(Damage==5)
  {
    bite2[2]=0;
    bite2[3]=0;
    bite2[4]=1;
    bite2[5]=1;
  }
  if(Damage==7)
  {
    bite2[2]=0;
    bite2[3]=1;
    bite2[4]=0;
    bite2[5]=0;
  }
  if(Damage==10)
  {
    bite2[2]=0;
    bite2[3]=1;
    bite2[4]=0;
    bite2[5]=1;
  }
  if(Damage==15)
  {
    bite2[2]=0;
    bite2[3]=1;
    bite2[4]=1;
    bite2[5]=0;
  }
  if(Damage==17)
  {
    bite2[2]=0;
    bite2[3]=1;
    bite2[4]=1;
    bite2[5]=1;
  }
  if(Damage==20)
  {
    bite2[2]=1;
    bite2[3]=0;
    bite2[4]=0;
    bite2[5]=0;
  }
  if(Damage==25)
  {
    bite2[2]=1;
    bite2[3]=0;
    bite2[4]=0;
    bite2[5]=1;
  }
  if(Damage==30)
  {
    bite2[2]=1;
    bite2[3]=0;
    bite2[4]=1;
    bite2[5]=0;
  }
  if(Damage==35)
  {
    bite2[2]=1;
    bite2[3]=0;
    bite2[4]=1;
    bite2[5]=1;
  }
  if(Damage==40)
  {
    bite2[2]=1;
    bite2[3]=1;
    bite2[4]=0;
    bite2[5]=0;
  }
  if(Damage==50)
  {
    bite2[2]=1;
    bite2[3]=1;
    bite2[4]=0;
    bite2[5]=1;
  }
  if(Damage==75)
  {
    bite2[2]=1;
    bite2[3]=1;
    bite2[4]=1;
    bite2[5]=0;
  }
  if(Damage==100)
  {
    bite2[2]=1;
    bite2[3]=1;
    bite2[4]=1;
    bite2[5]=1;
  }
  Serial.print(bite1[0]);
  Serial.print(bite1[1]);
  Serial.print(bite1[2]);
  Serial.print(bite1[3]);
  Serial.print(bite1[4]);
  Serial.print(bite1[5]);
  Serial.print(bite1[6]);
  Serial.print(bite1[7]);
  Serial.println("PlayerByte 1 gespeichert");

  Serial.print(bite2[0]);
  Serial.print(bite2[1]);
  Serial.print(bite2[2]);
  Serial.print(bite2[3]);
  Serial.print(bite2[4]);
  Serial.print(bite2[5]);
  Serial.println("PlayerByte 2 gespeichert");

  Serial.println(Player);
  //currentMillisCtf=millis();
}
/* Ab hier folgen die Unterprogramme für Capture the Flag sowie die Serielle- und Infrarotauswertung
 *  welche sich noch in Entwicklung befinden
/////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////Unterprogramm fÃ¼r den Spielmod CTF////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void ctfGame()
{
  int byteData1[8];     // zwischenspeicher fÃ¼r das Versenden von Datenbyte 1
  int byteData2[8];     // zwischenspeicher fÃ¼r das Versenden von Datenbyte 2
  int byteData3[8];
  int DataType=140;
  int DataEnd=232; 

  if(digitalRead(triggerPin)==LOW&&digitalRead(reloadPin)==LOW&&Pause==false)
  {
    for(int i=0; i>=7; i++) //Flag Anfangcode wird umgewandelt
    {
      byteData1[7-i]= DataType%2;
      DataType=DataType/2;
    }
    for(int i=0; i>=7; i++) //Flagnumber code wird umgewandelt
    {
      byteData2[7-i]= flagNumber%2;
      flagNumber=flagNumber/2;
    }
    for(int i=0; i>=7; i++) //End code wird umgewandelt
    {
      byteData3[7-i]= DataEnd%2;
      DataEnd=DataEnd/2;
    }
    
    if(indoorMode==true)
    {
     sendPulseIndoor(startBit, divisorstart);   //in der() gibt an welche zeit und der divisor dient zum ausgleich der abweichungen

     for(int i=0; i<=7; i++)   //DataTyp wird gesendet
     {
       if (byteData1[i] == 0)       
       {
         sendPulseIndoor(zeroBit, divisorzero);
       }
       else
       {
        sendPulseIndoor(oneBit,divisorone); 
       }
      }
     for(int i=0; i<=7; i++)   //Flagnumber wird gesendet
     {
       if (byteData2[i] == 0)       
       {
         sendPulseIndoor(zeroBit, divisorzero);  
       }
       else
       {
         sendPulseIndoor(oneBit,divisorone); 
       }
     }
     for(int i=0; i<=7; i++)   //Endbyte wird gesendet
     {
      if (byteData3[i] == 0)       
       {
        sendPulseIndoor(zeroBit, divisorzero);  
       }
      else
       {
        sendPulseIndoor(oneBit,divisorone); 
       }
      }
    }
    
    if(indoorMode==false)
    {
     sendPulse(startBit, divisorstart);   //in der() gibt an welche zeit und der divisor dient zum ausgleich der abweichungen

     for(int i=0; i<=7; i++)   //DataTyp wird gesendet
     {
       if (byteData1[i] == 0)       
       {
         sendPulse(zeroBit, divisorzero);
       }
       else
       {
        sendPulse(oneBit,divisorone); 
       }
      }
     for(int i=0; i<=7; i++)   //Flagnumber wird gesendet
     {
       if (byteData2[i] == 0)       
       {
         sendPulse(zeroBit, divisorzero);  
       }
       else
       {
         sendPulse(oneBit,divisorone); 
       }
     }
     for(int i=0; i<=7; i++)   //Endbyte wird gesendet
     {
      if (byteData3[i] == 0)       
       {
        sendPulse(zeroBit, divisorzero);  
       }
      else
       {
        sendPulse(oneBit,divisorone); 
       }
      }
    }
    Flag=false;
  } 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////Unterprogramm zur Umwandlung des Auswertungsschuss///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void scoreShoot()
{
  int byteSend3[8];
  int byteSend4[16];
  int byteSend5[16];
  int byteSend6[8];
  int DataEnd[8]={1,0,0,0,0,0,1,1};
  int weiter;
  
  if(DataRecv==false) 
  {
    digitalWrite(muzzleLed, HIGH);
    weiter=ammoUse;
    for(int i=0; i<=7; i++)  // sendet den Munitionsverbrauch pro SchuÃŸ
    {
      byteSend3[7-i]=ammoUse%2;
      weiter=weiter/2;

      if(byteSend3[i]==0)
      {
        sendPulseIndoor(zeroBit, divisorzero);
      }
      else
      {
        sendPulseIndoor(oneBit, divisorzero); 
      }
    }

    weiter=shots;
    for(int i=0; i<=15; i++)   // sendet die abgegebenen SchÃ¼ÃŸe
    {
      byteSend4[15-i]=shots%2;
      weiter=weiter/2;

      if(byteSend4[i]==0)
      {
        sendPulseIndoor(zeroBit, divisorzero);
      }
      else
      {
        sendPulseIndoor(oneBit, divisorzero); 
      }
    }

    for(int i=0; i<=32; i++)   //sendet die erhaltenen Treffer
    {
      weiter=enemyHits[i];
      for(int a=0; a<=15; a++)
      {
        byteSend5[15-a]=weiter%2;
        weiter=weiter/2;
        if(byteSend5[15-a]==0)
        {
          sendPulseIndoor(zeroBit, divisorzero);
        }
        else
        {
          sendPulseIndoor(oneBit, divisorzero); 
        }
      }
    }

    for(int i=0; i>=7; i++) //End code wird umgewandelt
    {
      if(DataEnd[i]==0)
      {
        sendPulseIndoor(zeroBit, divisorzero);
      }
      else
      {
        sendPulseIndoor(oneBit, divisorzero); 
      }
    }
    for(int i=0; i<=9; i++)
    { 
      recieveIR();
    }
  }
  if(DataRecv==true)   //ÃœberprÃ¼ft das die Daten an die Auswertung gegangen ist
  { 
    digitalWrite(muzzleLed, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Score Sended"));
    for(int i=0; i<=32; i++)
    {
      enemyHits[i]=0;
    }
    delay(3000);
    lcd.clear();
    DataRecv=false;
  }
  else
  {
    digitalWrite(muzzleLed, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("Score NOT Sended"));
    delay(3000);
    lcd.clear();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////recieveSerial///////////////////////////////////////////////////////
///////////////////Unterprogramm zum erhalten von Seriellen Daten/////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void recieveSerial()
{
  Serial.println("Holy Crap");
    char incName[14];
    for(int i=0; i<=13; i++)
    {  
      incName[i]=Serial.read();
    }
 
 ////////////Abfrage und Verarbeitung aller Spielernamen////////////////////////////////////////////////////////   
 if(Game==false)
 {
    if(isdigit(incName[0]))
    {
    for(int i; i<=10; i++)
    {
    Serial.println(incName[i]);
    }
     char pos[3];
     pos[0] = incName[0];
     pos[1] = incName[1];
     pos[2] = '\0';
     int ipos = atoi(pos);
     for(int i = 2; i <=13; i++)
     {
     enemyName[ipos][i-2]=incName[i];
     Serial.println(ipos);
     Serial.println(incName[i]);
      if(enemyName[ipos][i-2]==';')
      {
        enemyName[ipos][i-2]='\0';
        for(int a=12; a>i-2; a--)
        {
          enemyName[ipos][a]=' ';
        }
        Serial.println("break");
        break;
        
      } 
    } 
   Serial.println("AHHHHHHHHHHHHHH"); 
    }
    
/////// es folgt die Verarbeitung der Spielerdaten//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    if(incName[0]=='D'&& incName[1]=='a'&&incName[2]=='m'&&incName[3]=='a')
    {
     Serial.println("Damage");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     Damage=ipos;
     Serial.println(Damage);
    }
    if(incName[0]=='P'&&incName[1]=='l'&&incName[2]=='a'&&incName[3]=='y')
    {
     Serial.println("PlayerID");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     PlayerID=ipos;
     Player=enemyName[PlayerID];
     Serial.println(PlayerID);
     Serial.println(Player);
    }
    if(incName[0]=='T'&&incName[1]=='e'&&incName[2]=='a'&&incName[3]=='m')
    {
     Serial.println("Team");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     if(ipos>0&&ipos<5)
     {
     Team=ipos;
     }
     Serial.println(Team);
    }
    if(incName[0]=='L'&&incName[1]=='i'&&incName[2]=='f'&&incName[3]=='e')
    {
     Serial.println("Life");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     if(ipos>=0&&ipos<=99)
     {
     Life=ipos;
     }
     Serial.println(Life);
    }
    if(incName[0]=='A'&&incName[1]=='m'&&incName[2]=='m'&&incName[3]=='o')
    {
     Serial.println("Ammo");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     if(ipos>=1&&ipos<=999)
     { 
     Ammo=ipos;
     ammo=Ammo;
     }
     Serial.println(ammo);
    }
    if(incName[0]=='C'&&incName[1]=='l'&&incName[2]=='i'&&incName[3]=='p')
    {
     Serial.println("Clips");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     if(ipos>=0&&ipos<=99)
     {
     Clips=ipos;
     clips=Clips;
     }
     Serial.println(Clips);
    }
    if(incName[0]=='H'&&incName[1]=='e'&&incName[2]=='a'&&incName[3]=='h')
    {
     Serial.println("Health");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     if(ipos>=0&&ipos<=999)
     {
     Health=ipos;
     Serial.println(Health);
     }
    }
    if(incName[0]=='A'&&incName[1]=='m'&&incName[2]=='U'&&incName[3]=='s')
    {
     Serial.println("ammoUse");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     if(ipos>=1&&ipos<=10)
     {
     ammoUse=ipos;
     Serial.println(ammoUse);
     }
    }
    if(incName[0]=='R'&&incName[1]=='e'&&incName[2]=='T'&&incName[3]=='i')
    {
     Serial.println("respawnTime");
     char pos[4];
     pos[0] = incName[4];
     pos[1] = incName[5];
     pos[2] = incName[6];
     pos[3] = '\0';
     int ipos = atoi(pos);
     if(ipos>=1&&ipos<=500)
     {
     respawnTime=ipos;
     }
     Serial.println(respawnTime);
    }
    if(incName[0]=='F'&&incName[1]=='u'&&incName[2]=='A'&&incName[3]=='u')
    {
     Serial.println("fullAuto");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       fullAuto=false;
       Serial.println("fullAuto=false");
     }
     if(ipos==1)
     {
       fullAuto=true;
       Serial.println("fullAuto=true");
     }
    }
    if(incName[0]=='R'&&incName[1]=='e'&&incName[2]=='s'&&incName[3]=='p')
    {
     Serial.println("respawn");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       respawn=false;
       Serial.println("respawn=false");
     }
     if(ipos==1)
     {
       respawn=true;
       Serial.println("respawn=true");
     }
    }
    if(incName[0]=='F'&&incName[1]=='r'&&incName[2]=='F'&&incName[3]=='i')
    {
     Serial.println("friendlyFire");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       friendlyFire=false;
       Serial.println("friendlyFire=false");
     }
     if(ipos==1)
     {
       friendlyFire=true;
       Serial.println("friendlyFire=true");
     }
    }
    if(incName[0]=='C'&&incName[1]=='l'&&incName[2]=='U'&&incName[3]=='s')
    {
     Serial.println("clipUse");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       clipUse=false;
       Serial.println("clipUse=false");
     }
     if(ipos==1)
     {
       clipUse=true;
       Serial.println("clipUse=true");
     }
    }
    if(incName[0]=='I'&&incName[1]=='n'&&incName[2]=='M'&&incName[3]=='o')
    {
     Serial.println("indoorMode");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       indoorMode=false;
       Serial.println("indoorMode=false");
     }
     if(ipos==1)
     {
       indoorMode=true;
       Serial.println("indoorMode=true");
     }
    }
    if(incName[0]=='C'&&incName[1]=='t'&&incName[2]=='f'&&incName[3]=='!')
    {
     Serial.println("ctf");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       ctf=false;
       Serial.println("ctf=false");
     }
     if(ipos==1)
     {
       ctf=true;
       Serial.println("ctf=true");
     }
    }
    if(incName[0]=='F'&&incName[1]=='l'&&incName[2]=='F'&&incName[3]=='i')
    {
     Serial.println("flagFire");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       flagFire=false;
       Serial.println("flagFire=false");
     }
     if(ipos==1)
     {
       flagFire=true;
       Serial.println("flagFire=true");
     }
    }
 }
 
 
    if(incName[0]=='G'&&incName[1]=='a'&&incName[2]=='m'&&incName[3]=='e')
    {
     Serial.println("Game");
     char pos[2];
     pos[0] = incName[4];
     pos[1] = '\0';
     int ipos = atoi(pos);
     if(ipos==0)
     {
       Game=false;
       Serial.println("Game=false");
     }
     if(ipos==1)
     {
       Game=true;
       gameOver=false;
       Serial.println("Game=true");
     }
    }
  lcd.clear();  
}  

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////scoreSerial//////////////////////////////////////////////////////////////////////
////////////////Unterprogramm zum Senden der Scoredaten über Seriellen Anschluß////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void scoreSerial()
{
  Serial.println("UFFUFF");
  if(Serial.available()>0)
  {
 
 char c = Serial.read();
  if(c=='2')
  {
    //for(int i=0; i<=32; i++)
    //{
      //enemyHits[i]=20;
    //}
  Serial.print(PlayerID);
  Serial.print(";");
  delay(10);
  Serial.print(shots);
  Serial.print(";");
  delay(10);
  Serial.print(ammoUse*shots);
  Serial.print(";");
  delay(10);
  for(int i=1; i<=64; i++)
  {
    Serial.print(enemyHits[i]);
    Serial.print(";");
    delay(10);
  }
  Serial.println("Fertig");
  }
 
  if(c=='3')
    {
      pressedReloadPin=false;
      Serial.println("SCHUENUENUENUENUE");
      Serial.println("Fertig");
      shots=0;
      for(int i=1; i<=64; i++)
      {
        enemyHits[i]=0;
        Serial.println(enemyHits[i]);
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Sucessfully"));
      
      delay(2000);
      lcd.clear();
      lcd.setCursor(0,0);
    }
    }
  
}
*/




