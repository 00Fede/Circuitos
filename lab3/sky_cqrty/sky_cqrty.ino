#include <Wire.h>
#include "rgb_lcd.h"
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
byte estadoAlarma = 1; //estado estadoAlarmaa, 0 para inactiva, 1 para activa, default 0
byte estadoPuertaPrincipal = 0;  //Estado en que esta la puerta, 0 esta cerrada, 1 abierta
rgb_lcd lcd;
bool primeraVez;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10,11,12,13}; //connect to the column pinouts of the keypad

int DELAY_DISPARAR_ALARMA = 10; // Controla cantidad de segundos antes de disparar alarma

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
  pinMode(3,INPUT);
  pinMode(A0,INPUT);
  //lcd.print("hello world!");
  primeraVez=true;
  Serial.begin(9600);
}

String getAlarmState(){
  if(estadoAlarma){
    return "Activa";
  }
  return "Desactivada";
}

String getEstadoPuertaPrincipal(int sensorValue){
  if(sensorValue==1){
    estadoPuertaPrincipal = 1; // Estado puerta
  }else{
    estadoPuertaPrincipal = 0; // Estado puerta
  }
  if(estadoPuertaPrincipal){
    return "Abierta";
  }
  return "Cerrada";
}

void printEstados(int sensorPuerta){
  lcd.setCursor(0,0);
  lcd.print("Alarma: "+ getAlarmState());
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Puerta: "+ getEstadoPuertaPrincipal(sensorPuerta)); 
  delay(1000);
  lcd.clear();
}

void conteoPuertaAbierta(){
   for(int i = 0; i<DELAY_DISPARAR_ALARMA;i++){
      //sonar pitido
      delay(1000); 
      if(!estadoAlarma){
        return;
      } 
    }
}

char* toCharray(String str){
  int l = str.length();
  char *c = (char *) malloc(l);
  str.toCharArray(c,l+1);
  return c;
}

void printLCDScroll(String a){
   lcd.setCursor(16,0);
   //convertirStringenChar
   char b[a.length()+1];
   a.toCharArray(b,a.length()+1);
     
   for(int i=0;i < a.length();i++){
       lcd.print(b[i]);
       lcd.scrollDisplayLeft();
       delay(400);
    }

    lcd.clear();
}

String ingresarContrasena(int tipoCodigo){
  String codigo;
  String codigo_confirm;
  int botonFinalizar;
  lcd.clear(); //pone en ceros la pantalla
  switch(tipoCodigo){
    case 0:
    printLCDScroll("codigo_1 y oprima boton");
    break;
    case 1:
    printLCDScroll("codigo_2 y oprima boton");
    break;
    case 2:
    printLCDScroll("MASTER y oprima boton");
    break;
    case 3:
    printLCDScroll("Remoto y oprima boton");
    break;
  }
  lcd.setCursor(0,0);
  switch(tipoCodigo){
    case 0:
    lcd.print("Codigo_1");
    break;
    case 1:
    lcd.print("Codigo_2");
    break;
    case 2:
    lcd.print("Codigo Master");
    break;
    case 3:
    lcd.print("Codigo Remoto");
    break;
  }

  botonFinalizar = digitalRead(A0);
  Serial.println(botonFinalizar);

  lcd.setCursor(0,1);
  while(botonFinalizar==0){
    char customKey = customKeypad.getKey();
    if (customKey){
          lcd.print("*");
          codigo.concat(customKey);
    }
    botonFinalizar = digitalRead(A0);
  }

  delay(1000); //delay necesario para que pantalla reciba los cambios
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Confirmar codigo");
  botonFinalizar = 0;
  
  lcd.setCursor(0,1);
  while(botonFinalizar==0){
    char customKey = customKeypad.getKey();
    if (customKey){
        codigo_confirm.concat(customKey);
        lcd.print("*");
    }
    botonFinalizar = digitalRead(A0);
  }

  if(codigo.compareTo(codigo_confirm)==0){
    //son iguales
    return codigo;
  }else{
    return "";
  }
}

void configurarPrimeraVez(){

    String contrasenas[4];


    for(int i=0;i<4;i++){
      while(contrasenas[i]== ""){
        contrasenas[i]=ingresarContrasena(i);
      }
    }
    delay(100000);
  

}

void loop() {
  if(primeraVez){
    configurarPrimeraVez();
  }

  
  int sensorValue = digitalRead(3);
  
  printEstados(sensorValue); //Imprime estados de puerta, alarma, garaje y otros

  if(estadoPuertaPrincipal){
    conteoPuertaAbierta();
  }
  //prueba de teclado
  /**char customKey = customKeypad.getKey();
  
  if (customKey)
   Serial.println(customKey);*/
  }

