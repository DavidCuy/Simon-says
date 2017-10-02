#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "pitches.h"

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);                                                                                                    // Declarate de LCD element
int startTune[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};    // Melody of the beginning of the game
int startDuration[] = {100, 200, 100, 200, 100, 400, 100, 100, 100, 100, 200, 100, 500};                                                    // Duration of the tunes of the melody of the beginning of the game
int correctTune[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};                                                                   // Melody of the start of the game
int correctDuration[] = {100, 100, 100, 300, 100, 300};                                                                                        // Duration of the tunes of the melody of the start of the game
int pinNote[] = { NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4 };
int button[] = {4, 5, 6, 7};                                                                                                                // The four button input pins
int ledPin[] = {A3, A2, A1, A0};                                                                                                            // LED pins
int buttonState = 0;                                                                                                                        // Button state checker
int randomArray[100];                                                                                                                       // Intentionally long to store up to 100 inputs (doubtful anyone will get this far)
int speakerPin = 2;                                                                                                                         // Pin of speaker

byte record;
/**
 * Initialization of the arduino
 * Initialite LCD
 * Initialite LEDs pins
 * Initialite Buttons pins
 * Initialite random seed
 */
void setup() 
{
  record = EEPROM.read(0);
  lcd.begin(16, 2);
  lcd.print("=> Simon Dice <=");

  for(int x=0; x<4; x++)
    pinMode(ledPin[x], OUTPUT);
  
  for(int x=0; x<4; x++) {
    pinMode(button[x], INPUT);
    digitalWrite(button[x], HIGH);
  }

  randomSeed(analogRead(5));
  introTone();
  delay(1000);
}

 /**
  * Sequencial turns of the game
  */
void loop() 
{
  int auxTurn = 0;
  boolean repeatFlag = 0;
  for(int turn = 1; turn <= 100; turn++){
    if(repeatFlag){
      introTone();
      record = EEPROM.read(0);
      delay(1000);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Record: ");
    lcd.print(record, DEC);
    lcd.setCursor(0,1);
    lcd.print("Turno: ");
    lcd.print(turn);
    
    correctTone();
    randomArray[turn - 1] = random(0, 4);
    for(int i = 0; i < turn; i++){
      digitalWrite(ledPin[randomArray[i]], HIGH);
      tone(speakerPin, pinNote[randomArray[i]], 100);
      delay(400);
      digitalWrite(ledPin[randomArray[i]], LOW);
      delay(100);
    }
    delay(500);

    // The read button part
    auxTurn = 0;
    while(auxTurn != turn){
      boolean failFlag = 0;
      for(int i = 0; i < 4; i++){
        buttonState = digitalRead(button[i]);
        if(buttonState == LOW){
          digitalWrite(ledPin[i], HIGH);
          tone(speakerPin, pinNote[i], 100);
          delay(400);
          digitalWrite(ledPin[i], LOW);
          delay(100);
          if(randomArray[auxTurn] == i){
            auxTurn++;
          }
          else{
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print("**** ERROR ****");
            if(turn > record){
              EEPROM.write(0, turn);
            }
            failTone();
            failFlag = 1;
            turn = 0;
            break;
          }
        }
      }
      if(failFlag){
        repeatFlag = 1;
        break;
      }
      repeatFlag = 0;
    }
  }
}

void introTone(){
  for (int thisNote = 0; thisNote < 13; thisNote ++) {
     // play the next note:
     tone(speakerPin, startTune[thisNote], startDuration[thisNote]);
     // hold the note:
     if (thisNote==0 || thisNote==2 || thisNote==4 || thisNote== 6)
     {
       digitalWrite(ledPin[0], HIGH);
     }
     if (thisNote==1 || thisNote==3 || thisNote==5 || thisNote== 7 || thisNote==9 || thisNote==11)
     {
       digitalWrite(ledPin[1], HIGH);
     }
     if (thisNote==8 || thisNote==12)
     {
       digitalWrite(ledPin[2], HIGH);
     }  
     if (thisNote==10)
     {   
       digitalWrite(ledPin[3], HIGH);
     }
     delay(startDuration[thisNote]);
     // stop for the next note:
     noTone(speakerPin);
     for(int i = 0; i < 4; i++){
      digitalWrite(ledPin[i], LOW);
     }
     delay(25);
    }
}

void correctTone(){
  //function for generating the array to be matched by the player
  for(int i = 0; i < 4; i++){
    digitalWrite(ledPin[i], HIGH);
  }
  
  for (int thisNote = 0; thisNote < 6; thisNote ++) {
    // play the next note:
    tone(speakerPin, correctTune[thisNote], correctDuration[thisNote]);
    // hold the note:
    delay(correctDuration[thisNote]);
    // stop for the next note:
    noTone(speakerPin);
    delay(25);
  }

  for(int i = 0; i < 4; i++){
    digitalWrite(ledPin[i], LOW);
  }
  delay(1000);
}

void failTone() { //Function used if the player fails to match the sequence
  for (int y=0; y<=2; y++)
  { //Flashes lights for failure
    for(int i = 0; i < 4; i++){
      digitalWrite(ledPin[i], HIGH);
    }
    tone(speakerPin, NOTE_G3, 300);
    delay(200);
    for(int i = 0; i < 4; i++){
      digitalWrite(ledPin[i], LOW);
    }
    tone(speakerPin, NOTE_C3, 300);
    delay(200);
  }
  delay(500);
}

