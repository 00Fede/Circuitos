void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

#include "rgb_lcd.h"
#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
const byte numChars = 32;
boolean newData = false;


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12, 13}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char endMarker = '\n';


int getSizeOfSerialInput(){
  int count;
  while(Serial.read() != endMarker){
    count++;
  }
  Serial.print("Contador de serial ");
  Serial.println(count);
  return count;
}


char receivedChars[numChars];
void leerSerialAlEnter() {
    static byte ndx = 0;
    
    char rc;

    
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        Serial.println(rc);
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }else{
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0; //resets char array
            newData = true;

        }
    }
}

void getNewData() {
  Serial.println(newData);
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
    
}

void _getKey(){
  char key = customKeypad.getKey();
  if(key != NO_KEY){
    Serial.println(key);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
/**
  while(Serial.available()>0){
    Serial.println(Serial.available());
    Serial.print("char ");
    //Serial.println(Serial.read());
  }*/
  _getKey();
  /**int count;
  while(Serial.available()>0){
    Serial.print("char");
    Serial.println(Serial.read());
    //count++;
  }*/
  //Serial.print("Contador de serial ");
  //Serial.println(count);
  //char receivedChars[getSizeOfSerialInput()];
  
  //leerSerialAlEnter();
  //delay(1000);
  //getNewData();

  //Serial.println(receivedChars);
  

}

/**
 * Intento para contar caracteres de serial, antes de leerlos
 * cada vez que se da Serial.read() se desencola char de Serial.available();
 * serial.available va sumando los string metidos, y se resta a medida que se llama
 * Serial.read().
 */
/**while(Serial.available()>0 && !endCounting){
    Serial.println("Counting chars");
    char b = Serial.read();         //read first char
    while(b != endMarker){
      Serial.println("char read: ");
      Serial.print(b);
      count++;
      Serial.print(count);
      Serial.println(" chars counted");
      receivedString.concat(b);
      b = Serial.read();
    }
    endCounting = true;
    Serial.print("Finishing counting...");
    Serial.println(""); Serial.println("String obtained "); Serial.print(receivedString);
  }*/
