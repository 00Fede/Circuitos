#include <Wire.h>
#include "rgb_lcd.h"
#include <Keypad.h>
#include <Servo.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
byte estadoAlarma = 1; //estado estadoAlarmaa, 0 para inactiva, 1 para activa, default 0
byte estadoPuertaPrincipal = 0;  //Estado en que esta la puerta, 0 esta cerrada, 1 abierta
byte estadoGaraje = 0;            // Estado garaje, 0 cerrada, 1 abierta
rgb_lcd lcd;
bool primeraVez;
int error = 1;
Servo myServo;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12, 13}; //connect to the column pinouts of the keypad

int DELAY_DISPARAR_ALARMA = 10; // Controla cantidad de segundos antes de disparar alarma
boolean pantallaInfo = false;

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

String contrasenas[4];

String comando;

boolean ventanaComando = false;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  primeraVez = false; //SOLO PARA PRUEBAS, DEBE SER TRUE
  Serial.begin(9600);
  customKeypad.setHoldTime(3000);  // tiempo necesario para emitir evento HOLD
  customKeypad.addEventListener(keypadEvent);   //Listener de eventos para el key pad
  //attachInterrupt(4,botonPresionado,CHANGE);

  for (int i = 0; i < 4; i++) {
    contrasenas[i] = "123"; //SOLO PARA PRUEBAS, ELIMINAR
  }
}

void loop() {
  
  char key = customKeypad.getKey(); //necesario para que tome evento}
  if(key) Serial.println(key);
  
  if (!primeraVez) {
    delay(500);
    int b = digitalRead(4);
    while(b==0){
      limpiarVariables();
      delay(500);
      printEstados();
      b = digitalRead(4);
    }
    delay(500);
    ingresarComando();
  } else {
    configurarPrimeraVez();
  }
 
  //Serial.println(digitalRead(4));

  /**TODO
     if (estadoPuertaPrincipal) {
    conteoPuertaAbierta();
    }*/


}

void limpiarVariables() {
  error = 1;
  comando = "";
}

void keypadEvent(KeypadEvent key) {
  switch (customKeypad.getState()) {
    //case PRESSED:
      //if (key == '#' && pantallaInfo) ingresarComando();
      //break;
    case HOLD:
      if (ventanaComando && key=='#'){ 
        lcd.clear();
        avisoComandoIncorrecto();
        delay(500);
        lcd.clear();
        printEstados();
      }
  }
}

// Muestra display para entrar comando.
void ingresarComando() {
  ventanaComando = true;
  delay(500);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  comando = "";
  String msg = "Comando ";
  msg = msg + error;
  lcd.print(msg);
  Serial.println(digitalRead(4));

  pantallaInfo = false;

  lcd.setCursor(0, 1);
  int button = digitalRead(4);
  while (ventanaComando && button == 0) {
    char key = customKeypad.getKey();
    if (key) {
      comando.concat(key);
      lcd.print(key);
    }
    button = digitalRead(4);
  }
  procesarComando(comando);
  lcd.clear();
}

void botonPresionado() {
  Serial.println("boton pressed");
  if (ventanaComando) {
    procesarComando(comando);
  }
}

void restablecer() {
  String comando;
  lcd.clear(); lcd.setCursor(0, 0); lcd.print("Ingrese Master");
  int botonCancelar = digitalRead(4);
  lcd.setCursor(0, 1);
  while (botonCancelar == 0) {
    char key = customKeypad.getKey();
    if (key) {
      comando.concat(key);
      lcd.print(key);
    }
    botonCancelar = digitalRead(4);
  }
  primeraVez = comando.compareTo(contrasenas[2]) == 0;
}

void checkErrores() {
  if (error == 4) {
    Serial.println("ingrese master");
    restablecer();
  } else {
    ingresarComando();
  }
}

void procesarComando(String comando) {
  char tipo = comando.charAt(0);
  comando = comando.substring(1, comando.length());
  Serial.println(comando);
  lcd.clear(); lcd.setCursor(0, 0);
  switch (tipo) {
    case 'A':
      if (comando.compareTo(contrasenas[0]) == 0) {
        avisoComandoCorrecto(tipo);
        estadoAlarma = 1;
        Serial.println("Alarma activada");
        printEstados();
      } else {
        avisoComandoIncorrecto();
        error++;
        checkErrores();
      }
      break;
    case 'B':
      if (comando.compareTo(contrasenas[0]) == 0) {
        avisoComandoCorrecto(tipo);
        estadoAlarma = 0;
        Serial.println("Alarma desactivada");
        printEstados();
      } else {
        avisoComandoIncorrecto();
        error++;
        checkErrores();
      }
      break;
    case 'C':
      if (comando.compareTo(contrasenas[1]) == 0) {
        avisoComandoCorrecto(tipo);
        estadoGaraje = 1;
        Serial.println("Abriendo garaje");
        printEstados();
      } else {
        avisoComandoIncorrecto();
        error++;
        checkErrores();
      }
      break;
     case 'D':
      if (comando.compareTo(contrasenas[1]) == 0) {
        avisoComandoCorrecto(tipo);
        estadoGaraje = 0;
        Serial.println("Cerrando garaje");
        printEstados();
      } else {
        avisoComandoIncorrecto();
        error++;
        checkErrores();
      }
     default:
      avisoComandoIncorrecto();
      error++;
      checkErrores();
  }
}

void avisoComandoCorrecto(char tipo) {
  switch (tipo) {
    case 'A':
      lcd.print("Alarma activada");
      break;
    case 'B':
      lcd.autoscroll();
      lcd.print("Alarma desactivada");
      lcd.noAutoscroll();
      break;
     case 'C':
      lcd.print("Abriendo garaje");
      break;
     case 'D':
      lcd.print("Cerrando garaje");
      break;
  }
  delay(1000);
  lcd.clear();
}

void avisoComandoIncorrecto() {
  lcd.print("Incorrecto");
  delay(500);
}


String getState(byte state) {
  if (state) {
    return "ON";
  }
  return "OFF";
}

String getEstadoPuertaPrincipal(int sensorValue) {
  if (sensorValue == 1) {
    estadoPuertaPrincipal = 1; // Estado puerta
  } else {
    estadoPuertaPrincipal = 0; // Estado puerta
  }
  if (estadoPuertaPrincipal) {
    return "ON";
  }
  return "OFF";
}

void printEstados() {
  limpiarVariables();
  int sensorValue = digitalRead(3);
  lcd.setCursor(0, 0);
  lcd.print("Alarm " + getState(estadoAlarma));
  lcd.setCursor(0, 1);
  lcd.print("Door " + getEstadoPuertaPrincipal(sensorValue));
  lcd.print(" Gje " + getState(estadoGaraje));
  pantallaInfo = true;
}

void conteoPuertaAbierta() {
  for (int i = 0; i < DELAY_DISPARAR_ALARMA; i++) {
    //sonar pitido
    delay(1000);
    if (!estadoAlarma) {
      return;
    }
  }
}

void printLCDScroll(String a) {
  lcd.setCursor(16, 0);
  char b[a.length() + 1];
  a.toCharArray(b, a.length() + 1);

  for (int i = 0; i < a.length(); i++) {
    lcd.print(b[i]);
    lcd.scrollDisplayLeft();
    delay(250);
  }
  lcd.clear();
}

String ingresarContrasena(int tipoCodigo) {
  String codigo;
  String codigo_confirm;
  int botonFinalizar;
  lcd.clear(); //pone en ceros la pantalla
  switch (tipoCodigo) {
    case 0:
      printLCDScroll("Codigo_1 y oprima boton");
      break;
    case 1:
      printLCDScroll("Codigo_2 y oprima boton");
      break;
    case 2:
      printLCDScroll("MASTER y oprima boton");
      break;
    case 3:
      printLCDScroll("Remoto y oprima boton");
      break;
  }
  lcd.setCursor(0, 0);
  switch (tipoCodigo) {
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

  botonFinalizar = digitalRead(4);
  Serial.println(botonFinalizar);

  lcd.setCursor(0, 1);
  while (botonFinalizar == 0) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      lcd.print("*");
      codigo.concat(customKey);
    }

    botonFinalizar = digitalRead(4);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  delay(500);

  //Verifica longitud de codigo
  if (tipoCodigo == 2) {
    if (codigo.length() < 10) {
      lcd.print("Longitud < 10");
      delay(1000);
      return "";
    }
  } else {
    if (codigo.length() < 6) {
      lcd.print("longitud < 6");
      delay(1000);
      return "";
    }
  }
  lcd.print("Confirmar codigo");
  botonFinalizar = 0;
  lcd.setCursor(0, 1);
  while ( botonFinalizar == 0 ) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      codigo_confirm.concat(customKey);
      lcd.print("*");
    }
    botonFinalizar = digitalRead(4);
  }

  if (codigo.compareTo(codigo_confirm) == 0) return codigo;
  return "";
}



void configurarPrimeraVez() {
  for (int i = 0; i < 4; i++) {
    while (contrasenas[i] == "") {
      contrasenas[i] = ingresarContrasena(i);
    }
  }
  primeraVez = 0;
  delay(1000);
}



