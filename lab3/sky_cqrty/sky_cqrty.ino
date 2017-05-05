#include <Wire.h>
#include "rgb_lcd.h"
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
byte alarm = 1; //estado alarma, 0 para inactiva, 1 para activa, default 0
rgb_lcd lcd;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10,11,12,13}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  pinMode(3,INPUT);
  //lcd.print("hello world!");
  Serial.begin(9600);
}

String getAlarmState(){
  if(alarm){
    return "Activa";
  }
  return "Desactivada";
}

void loop() {
  int sensorValue = digitalRead(3);
  if(sensorValue==1){
    //open door
  }else{
    // close the door
  }
  lcd.setCursor(0,0);
  //esta activa
  getAlarmState();
  lcd.print("Alarma: "+ getAlarmState());

// prueba de teclado
  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey);
  }
}
