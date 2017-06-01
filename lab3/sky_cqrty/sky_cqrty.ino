//#include <Wire.h>
//#include "rgb_lcd.h"
#include <Servo.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <SD.h>

// Constantes para pantalla TFT
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8
#define SD_CS    4  // Chip select line for SD card
#define BUTTON_NONE 0
#define BUTTON_DOWN 1
#define BUTTON_RIGHT 2
#define BUTTON_SELECT 3
#define BUTTON_UP 4
#define BUTTON_LEFT 5

/** CONSTANTES */
//const byte ROWS = 4;                  //four rows
//const byte COLS = 4;                  //four columns
byte grados = 90;                // constante de rotacion
byte DELAY_DISPARAR_ALARMA = 10;     // Controla cantidad de segundos antes de disparar alarma

/** ESTADOS */
boolean estadoAlarma = true;              //estado estadoAlarmaa, 0 para inactiva, 1 para activa, default 1
boolean estadoPuertaPrincipal = false;     //Estado en que esta la puerta, 0 esta cerrada, 1 abierta
boolean estadoGaraje = false;              // Estado garaje, 0 cerrada, 1 abierta

/** DISPOSITIVOS */
//rgb_lcd lcd;
Servo myServo;

/** BANDERAS */
boolean primeraVez;
boolean pantallaInfo = false;
boolean ventanaComando = false;
boolean newData = false;
boolean remoto = false;                   //indica cuando se abrio una sesión remota
boolean leyo = false;           //Bandera para saber cuando leerSerial leyo una linea

/** VARIABLES */
String contrasenas[4];
String comando;
byte error = 1;
char receivedChars[32];
byte pos = 0;                    // posicion actual del cursor.
byte rebote = 0;                // maneja el antirebote en el boton analogo


/** PINES */
int pinServo = 5;
int pinBoton = A0;
int pinBuzzer = 4;
int pinSensorPuerta = 3;

/** INICIALIZACIÓN PERIFERICOS */
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

void setup() {
  // put your setup code here, to run once:
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  pinMode(pinSensorPuerta, INPUT);
  pinMode(pinBoton, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  Serial.begin(9600);

  tft.fillScreen(ST7735_BLACK);
   printEstadosTFT();
   
  //PRUEBA
  primeraVez = false; //SOLO PARA PRUEBAS, DEBE SER TRUE
  contrasenas[0] = "12345"; //cod1
  contrasenas[1] = "12345"; //cod2
  contrasenas[2] = "12345"; //master
  contrasenas[3] = "12345"; //remoto
  Serial.println("Initializing SD card...");
 if (!SD.begin(SD_CS)) {
    Serial.println("failed!");
    return;
  }
}
void loop() {
  uint8_t b = readButton();

  if(b == BUTTON_DOWN){
    itemSelected(pos);  // se mueve a posición, dibuja celda.
    pos = (pos+1) % 3;  // avanza uno en la posicion
  }
  if(b == BUTTON_LEFT){
    
  }
  if(b == BUTTON_UP){
    
  }
  if(b == BUTTON_RIGHT){
    
  }
  if(b == BUTTON_SELECT){
    
  }

}

//logica al seleccionar item en lista
void itemSelected(uint8_t pos){
  Serial.println(pos);
  if(pos == 0){
    //cursor sobre opcion alarma
    tft.setCursor(0,17*pos); // posicion del cursor para escribir
    tft.fillRect(0,17*pos, tft.width(), 16, ST7735_WHITE); // dibuja un rect en posicion de pos, color blanco
    drawtext("Alarm:",ST7735_BLACK,2,ST7735_WHITE);
  }if(pos == 1){
    //cursor sobre opcion door
    
  }
  if(pos == 2){
    //cursor sobre opcion garage
    
  }
  
  
}

// Lee señal de analogo y retorna numero (segun dirección)
uint8_t readButton(void) {
  float a = analogRead(3);
  byte retorno;
  
  a *= 5.0;
  a /= 1024.0;
  
  Serial.print("Button read analog = ");
  Serial.println(a);
  if (a < 0.2) {retorno = BUTTON_DOWN;}
  else if(a < 1.0) {retorno = BUTTON_RIGHT;}
  else if(a < 1.5) {retorno = BUTTON_SELECT;}
  else if (a < 2.0) {retorno = BUTTON_UP;}
  else if (a < 3.2) {retorno = BUTTON_LEFT;}
  else {retorno = BUTTON_NONE;}
  
  if(rebote==2){
    Serial.println("retorno ");
    Serial.println(retorno);
    rebote = 0; // Se reinicia contador de rebote
    return retorno; // retorne lo que reboto
  }else{
    Serial.println(rebote);
    rebote++; // Si no ha llegado rebote limite, siga sumando ando
  }
}

void leerSerial() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  while (Serial.available() > 0 && !newData ) {          //ciclo mientenras haya algo disponible en serial y no sea algo nuevo
    char rc = Serial.read();                            //obtiene primer char del serial
    //Serial.println(rc);
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= 32) {
        ndx = 32 - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0';            // terminate the string
      ndx = 0;                              //resets char array
      // return;                               // retorna y entrega el char[] con la lectura; No sirve, se necesita condicional adicional ademas de serial.available()
      newData = true;                       // Indica que ya se termino de leer la linea y la newData llego!
      leyo = true;
    }
  }
  newData = false;                          // Bandera en falso permite que proxima linea pueda ser leida

}

void empiezaSesionRemota() {
  Serial.println("Entro en sistema remoto, bienvenido");
  if (!checkPassword(3)) return;    // check remote pass and return true if correct, false otherwise

  Serial.println("Contraseña Correcta, Ingresando...");
  delay(1000);
  Serial.println("Ingrese comando");
  Serial.println("Escriba help para recibir ayuda");

  while (remoto) {                              // Mientras no se quiera salir del remoto
    leerSerial();                               // lea instruccion
    procesaInstruccion();                       // procese la Instruccion
  }

}
/**
   Checks password entrance in remote session. da 3 intentos, despues sale para volver a ingresar
   comando remoto. retorna true si se validan pass, false otherwise
*/
boolean checkPassword(int tipo) {
  int checkErrors = 1;      //contador del numero de intentos de ingreso contraseña
  leyo = false;         // puedo leer
  while (checkErrors < 4) {   //Mientras que los intentos de pass sean menores a 4
    Serial.println("\nIngrese contraseña"); Serial.print("Intento "+checkErrors);
    memset(receivedChars, 0, sizeof(receivedChars));    // limpia el receivedChars array antes de leer contraseña
    while (!leyo) {
      leerSerial();       //lee contrasñea
    }
    leyo = false;       //para volver a leer luego
    if (contrasenas[tipo]==receivedChars) {
      return true; //si contraseña correcta
    } else {
      Serial.println("contraseña incorrecta!");
      checkErrors++;          // Aumenta contador de intentos
    }
  }
  Serial.println("numero de intentos excedido");
  // se llamaria restablecer aqui
  remoto = false;       //lo expulsa del remoto, debe volver a ingresar.
  return false; //Si sale del while es porque excedio num de intentos
}

/**
   Coge instruccion recibida en receivedChars y ejecuta la accion correspondiente
*/
void procesaInstruccion() {

  String instruccion = receivedChars;
  delay(100); //needed for stability
  if (instruccion.compareTo("help") == 0) {
    Serial.println("Muestre ayuda");
    Serial.print("\n Comandos:\nActivar Alarma -> activarAlarma\nDesactivar Alarma -> desactivarAlarma\nAbrir garaje -> abrirGaraje\nCerrar Garaje -> cerrarGaraje\n");
  } else if (instruccion.compareTo("exit") == 0) {
    Serial.println("salir del remoto");
    remoto = false;       //pone bandera remoto en falso para salir del loop y volver a loop ppal
  } else if (instruccion.compareTo("activarAlarma") == 0) {
    Serial.println("Instruccion activarAlarma");
    if (!checkPassword(0)) return;              // lee pass de serial, valida con tipo y retorna si no pasa
    estadoAlarma = true;                           //Cambia el estado de la alarma
    printEstadosTFT();                            // Muestra pantalla principal con estados actualizados
    Serial.println("Alarma activada.");         // Aviso confirmacion a pantalla remoto
  } else if (instruccion.compareTo("desactivarAlarma") == 0) {
    Serial.println("Instruccion desactivarAlarma");
    if (!checkPassword(0)) return;              // lee pass de serial, valida con tipo y retorna si pasa
    estadoAlarma = false;
    printEstadosTFT();                  
    Serial.println("Alarma desactivada.");         // Aviso confirmacion a pantalla remoto
  } else if (instruccion.compareTo("abrirGaraje") == 0) {
    Serial.println("Instruccion abrirGaraje");
    if (!checkPassword(1)) return;            // lee pass del serial, valida con tipo de codigo, y retorna si no pasa
    if (!estadoGaraje) accionarGaraje(true);  //Si garaje no esta previamente abierto, abralo, en accionarGaraje cambia el estado
    printEstadosTFT();                  
    Serial.println("Abriendo Garaje");
  } else if (instruccion.compareTo("cerrarGaraje") == 0) {
    Serial.println("Instruccion cerrarGaraje");
    if (!checkPassword(1)) return;            // lee pass del serial, valida con tipo de codigo, y retorna si no pasa
    if (estadoGaraje) accionarGaraje(false);   //Si garaje no estaba previamente cerrado, cierrelo, en accionar Garaje cambia el estado
    printEstadosTFT();                  
    Serial.println("Cerrando Garaje");
  } else if (instruccion.compareTo("cambiarContrasenas") == 0) {
    Serial.println("Instruccion cambiarContrasenas");
  }
  memset(receivedChars, 0, sizeof(receivedChars));    // limpia el comando ingresado para que no sea leido luego
  
}

void accionarGaraje(boolean direccion) {
  myServo.attach(pinServo);
  if (direccion) {
    estadoGaraje = true;
    for (int i = 0; i <= grados; i++) {
      myServo.write(i);
      delay(15);
    }
  } else {
    estadoGaraje = false;
    for (int i = grados; i > 0; i--) {
      myServo.write(i);
      delay(15);
    }
  }
  delay(100);
  myServo.detach();

}

String getState(byte state) {
  if (state) {
    return "ON";
  }
  return "OFF";
}

String getEstadoPuertaPrincipal(int sensorValue) {
  estadoPuertaPrincipal = sensorValue == 1;
  if (estadoPuertaPrincipal) {
    return "ON";
  }
  return "OFF";
}

void drawtext(String text, uint16_t color, int tam, uint16_t background) {
  tft.setTextSize(tam);
  tft.setTextColor(color,background);
  tft.println(text);
}

void printEstadosTFT() {
  tft.fillScreen(ST7735_BLACK);  // limpia el display, poniendo el fondo negro.
  limpiarVariables();
  
  int sensorValue = digitalRead(pinSensorPuerta);
  //String texto = "\nAlarm \n" + getState(estadoAlarma) +
                 "\nDoor \n" + getEstadoPuertaPrincipal(sensorValue) +
                 "\nGarage \n" + getState(estadoGaraje);
  tft.setCursor(0, 0);
  drawtext("Alarm:", ST7735_WHITE, 2, ST7735_BLACK);
  if(getState(estadoAlarma)=="ON"){
    drawtext("ON", ST7735_GREEN, 3, ST7735_BLACK);  
  }else if(getState(estadoAlarma)=="OFF"){
    drawtext("OFF", ST7735_RED, 3, ST7735_BLACK);  
  }

  drawtext("Door:", ST7735_WHITE, 2, ST7735_BLACK);
  if(getEstadoPuertaPrincipal(sensorValue)=="ON"){
    drawtext("ON", ST7735_GREEN, 3, ST7735_BLACK);  
  }else if(getEstadoPuertaPrincipal(sensorValue)=="OFF"){
    drawtext("OFF", ST7735_RED, 3, ST7735_BLACK);  
  }  
  drawtext("Garage:", ST7735_WHITE, 2, ST7735_BLACK);
  if(getState(estadoGaraje)=="ON"){
    drawtext("ON", ST7735_GREEN, 3, ST7735_BLACK);  
  }else if(getState(estadoGaraje)=="OFF"){
    drawtext("OFF", ST7735_RED, 3, ST7735_BLACK);  
  }    
  
  pantallaInfo = true;
}

void limpiarVariables() {
  error = 1;
  comando = "";
}


