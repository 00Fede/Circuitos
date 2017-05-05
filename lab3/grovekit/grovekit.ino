#include <Wire.h>
#include "rgb_lcd.h"

const int colorR = 0;
const int colorG = 0;
const int colorB = 255;
int thresholdvalue=400;       //the threshold to turn on or off the LED

rgb_lcd lcd;

void setup()
{
  // set up the LCD's number of columns and rows: cols, rows
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);
     // Print a message to the LCD.
}

void loop()
{
  lcd.setCursor(0, 1);
  int sensorValue = analogRead(0); //the light sensor is attached to analog 0
    if(sensorValue>thresholdvalue)
    {
        lcd.print("Mucha luz");
        lcd.setCursor(0,0);
        lcd.print(sensorValue);
    }
    else
    { lcd.print("Poca luz");
        lcd.setCursor(0,0);
        lcd.print(sensorValue);
       
    }

    delay(1000);
}
