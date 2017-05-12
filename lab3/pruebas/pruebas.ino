#include <Wire.h>
#include "rgb_lcd.h"
#include <Keypad.h>

rgb_lcd lcd;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12, 13}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  Serial.begin(9600);
  customKeypad.setHoldTime(3000);  // tiempo necesario para emitir evento HOLD
  customKeypad.addEventListener(keypadEvent);   //Listener de eventos para el key pad
  

}

void loop() {
  // put your main code here, to run repeatedly:
  char key = customKeypad.getKey(); //necesario para que tome evento}
  if(key) Serial.println(key);

}

void keypadEvent(KeypadEvent key) {
  switch (customKeypad.getState()) {
    case PRESSED:
      Serial.print("presionado");
      //delay(500);
      //if (key == '#' && pantallaInfo) ingresarComando();
      break;
    case HOLD:
      Serial.print("sostenido");
      //delay(500);
      //if (key == '#' && ventanaComando) procesarComando(comando);
  }
}
