#include <Wire.h>
#include "rgb_lcd.h"
#include <Keypad.h>
#include <Servo.h>

/** CONSTANTES */
const byte ROWS = 4;                  //four rows
const byte COLS = 4;                  //four columns
const int grados = 90;                // constante de rotacion
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
const int DELAY_DISPARAR_ALARMA = 10;     // Controla cantidad de segundos antes de disparar alarma
const byte numChars = 32;



/** ESTADOS */
byte estadoAlarma = 1;              //estado estadoAlarmaa, 0 para inactiva, 1 para activa, default 0
byte estadoPuertaPrincipal = 0;     //Estado en que esta la puerta, 0 esta cerrada, 1 abierta
byte estadoGaraje = 0;              // Estado garaje, 0 cerrada, 1 abierta

/** DISPOSITIVOS */
rgb_lcd lcd;
Servo myServo;

/** BANDERAS */
bool primeraVez;
boolean pantallaInfo = false;
boolean ventanaComando = false;
boolean newData = false;
boolean remoto = false;                   //indica cuando se abrio una sesión remota
boolean salirRemoto;

/** VARIABLES */
String contrasenas[4];
String comando;
int error = 1;
char receivedChars[numChars];

/** VARIABLES SONIDO (BETA)*/
int _length[] = {15, 5, 4};
char notas[] = {"ccggaagffeeddc "};
char sonidoCorrecto[] = {"C"};
char sonidoIncorrecto[] = {"g"};
int beats_correcto[] = {1, 2};
int beats_incorrecto[] = {1, 2};
int beats[] = {1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4};
int tempo = 800;

/** PINES */ 
int pinServo = 5;
int pinBoton = A0;
int pinBuzzer = 4;
byte rowPins[ROWS] = {6, 7, 8, 9};     //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 11, 12, 13}; //connect to the column pinouts of the keypad


Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(3, INPUT);
  pinMode(pinBoton, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  primeraVez = false; //SOLO PARA PRUEBAS, DEBE SER TRUE
  Serial.begin(9600);
  customKeypad.setHoldTime(3000);  // tiempo necesario para emitir evento HOLD
  customKeypad.addEventListener(keypadEvent);   //Listener de eventos para el key pad
  //attachInterrupt(4,botonPresionado,CHANGE);

  //PRUEBA
  contrasenas[0] = "12345"; //cod1
  contrasenas[1] = "12345"; //cod2
  contrasenas[2] = "12345"; //master
  contrasenas[3] = "12345"; //remoto
}



void loop() {

  if (!primeraVez && !remoto) {

    Serial.print("Entro en primera vez false");

    // obtiene valores de disparadores
    char key = customKeypad.getKey();
    int b = digitalRead(pinBoton);
    int sensorValue = digitalRead(3);


    Serial.print("Estado puerta ppal: ");
    Serial.println(estadoPuertaPrincipal);
    Serial.print("Estado boton: ");
    Serial.println(b);
    Serial.println(key);

    //Imprime la información ppal
    limpiarVariables();
    printEstados();

    if (key == NO_KEY) {
      //Estado: Remoto

      empiezaSesionRemota();          //For testing purposes

      
      switch (key) {
        case '*':
          remoto = true;                  //Indica que se ha iniciado una sesion remota
          lcdPrint("Remoto Activo",0);    // Imprime en lcd el estado de la sesion remota
          empiezaSesionRemota();          //Contiene la lógica relativa a la sesion remota
          break;
      }
    } else if (b != 0) {
      //Estado: Ingresar Comando
      ingresarComando();
    } else if (sensorValue != 0) {
      //Estado: Puerta Abierta
      conteoPuertaAbierta();
    }
  } else {
    configurarPrimeraVez();
  }

}

//Limpia todo el lcd e imprime a en columna 0
void lcdPrint(String a, int b){
    if(b<2){
      lcd.clear();
      lcd.setCursor(0,b);
      lcd.print(a);
    }
}


void leerSerial() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while(Serial.available()>0 && !newData){              //ciclo mientenras haya algo disponible en serial y no sea algo nuevo
    Serial.print("Entro a lectura, available ");
    Serial.print(Serial.available());
    char rc = Serial.read();                            //obtiene primer char del serial
    Serial.println(rc);
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0';            // terminate the string
      ndx = 0;                              //resets char array
      // return;                               // retorna y entrega el char[] con la lectura; No sirve, se necesita condicional adicional ademas de serial.available()
      newData = true;                       // Indica que ya se termino de leer la linea y la newData llego!
    }
  }
 newData = false;                          // Bandera en falso permite que proxima linea pueda ser leida
}

void empiezaSesionRemota(){
  Serial.println("Entro en sistema remoto, bienvenido");
  Serial.println("Ingrese contraseña");

  leerSerial();                   // Lee una linea de serial y la guarda en receivedChars char array
  Serial.print("Value in receivedChars ");Serial.println(receivedChars);

  if(contrasenas[3]!=receivedChars){            // Verifica que contraseña remoto sea correcta
    Serial.println("Contraseña incorrecta");
    return;
  }

  Serial.println("Contraseña Correcta, Ingresando...");
  delay(1000);
  Serial.println("Ingrese comando");
  Serial.println("Escriba help para recibir ayuda");
  salirRemoto = false;
  
  while(!salirRemoto){                 // Mientras no se quiera salir del remoto
    leerSerial();                               // lea instruccion
    procesaInstruccion();                       // procese la Instruccion
  }
  
  Serial.print("Value in receivedChars ");Serial.println(receivedChars);

}

/**
 * Coge instruccion recibida en receivedChars y ejecuta la accion correspondiente
 */
void procesaInstruccion(){

    String instruccion = receivedChars;
    Serial.print("\n Value in instruccion ");Serial.println(instruccion);


    if(instruccion.compareTo("help")==0){
      Serial.println("Muestre help");
    }else if(instruccion.compareTo("exit")==0){
      Serial.println("salir del remoto");
      salirRemoto = true;
    }else if(instruccion.compareTo("abrirPuerta")==0){
      Serial.println("Instruccion abrir puerta");
    }    
}

String trimCharArray(char a[]){
  int sizeString = sizeof(a);
  Serial.println("tamaño de char array");
  Serial.print(sizeString);
  String charTrimmed;
  for(int i = 0; i<sizeString;i++){
    if(a[i] != ' '){
      charTrimmed.concat(a[i]);
    }
    if(a[i+1] == ' '){
      return charTrimmed;
    }
  }
}

void keypadEvent(KeypadEvent key) {
  switch (customKeypad.getState()) {
    //case PRESSED:
    //if (key == '#' && pantallaInfo) ingresarComando();
    //break;
    case HOLD:
      if (ventanaComando && key == '#') {
        lcd.clear();
        avisoComandoIncorrecto();
        delay(500);
        lcd.clear();
        printEstados();
      }
  }
}


/* play tone */
void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(pinBuzzer, HIGH);
    delayMicroseconds(tone);
    digitalWrite(pinBuzzer, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 2093, 2349, 2637, 2794, 3136, 3520, 3951, 4186};

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void reproducirSonido(char sonido[], int beats[], int _length) {
  for (int i = 0; i < _length; i++) {
    if (sonido[i] == ' ') {
      delay(beats[i] * tempo);
    } else {
      playNote(sonido[i], beats[i] * tempo);
    }
    delay(tempo / 2);    /* delay between notes */
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
  Serial.println(digitalRead(pinBoton));

  pantallaInfo = false;

  lcd.setCursor(0, 1);
  int button = digitalRead(pinBoton);
  while (ventanaComando && button == 0) {
    char key = customKeypad.getKey();
    if (key) {
      comando.concat(key);
      lcd.print(key);
    }
    button = digitalRead(pinBoton);
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
  int botonCancelar = digitalRead(pinBoton);
  lcd.setCursor(0, 1);
  while (botonCancelar == 0) {
    char key = customKeypad.getKey();
    if (key) {
      comando.concat(key);
      lcd.print(key);
    }
    botonCancelar = digitalRead(pinBoton);
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
        if (estadoGaraje != 1) accionarGaraje(true);
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
        if (estadoGaraje != 0) accionarGaraje(false);
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

void accionarGaraje(boolean direccion) {
  myServo.attach(pinServo);
  if (direccion) {
    estadoGaraje = 1;
    for (int i = 0; i <= grados; i++) {
      myServo.write(i);
      delay(15);
    }
  } else {
    estadoGaraje = 0;
    for (int i = grados; i > 0; i--) {
      myServo.write(i);
      delay(15);
    }
  }
  delay(100);
  myServo.detach();

}

void avisoComandoCorrecto(char tipo) {
  reproducirSonido(sonidoCorrecto, beats_correcto, _length[1]);
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
  reproducirSonido(sonidoIncorrecto, beats_incorrecto, _length[2]);
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
void limpiarVariables() {
  error = 1;
  comando = "";
}

void conteoPuertaAbierta() {
  for (int i = 0; i < DELAY_DISPARAR_ALARMA; i++) {
    //sonar pitido
    delay(1000);
    if (!estadoAlarma) {
      return;
    }
  }
  reproducirSonido(notas, beats, _length[0]);
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

  botonFinalizar = digitalRead(pinBoton);
  Serial.println(botonFinalizar);

  lcd.setCursor(0, 1);
  while (botonFinalizar == 0) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      lcd.print("*");
      codigo.concat(customKey);
    }

    botonFinalizar = digitalRead(pinBoton);
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
    botonFinalizar = digitalRead(pinBoton);
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




