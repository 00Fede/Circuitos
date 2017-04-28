#include <TimerOne.h>

// Declaramos pines a usar para 74ls48
int TABLERO_SIZE = 7;
int ABCD_SIZE = 4;
int abcd[] = {4,5,6,7};
int BOTON1 = 2;
int BOTON2 = 3;
int count;
int countleft = 0;
int countright = 0;
boolean iniciar_conteo = false;
int precount = 3;
// del 14 al 19 corresponde a salidas analogas A0...A5
int tablero[] = {12,14,15,16,17,18,19}; // leds del tablero

// Variables will change:
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 25;    // the debounce time; increase if the output flickers

int buttonState1;             // the current reading from the input pin
int lastButtonState1 = LOW;   // the previous reading from the input pin

// indica cuando inicia el juego
boolean start = false;

void setup() {
  // put your setup code here, to run once:
  for(int i=0;i<ABCD_SIZE;i++){
    pinMode(abcd[i],OUTPUT);
  }
  pinMode(BOTON1, INPUT);
  pinMode(BOTON2,INPUT);
  //Inicializa los led del tablero
  for(int i = 0; i<TABLERO_SIZE;i++){
    pinMode(tablero[i],OUTPUT);
  }
  
  Serial.begin(9600); //Se inicia el serial
  
  countleft = 0;//reinicia contadores
  countright = 0;
  apagar7seg();
  apagarTodos();

}


// Timer Service Routine
void timerIsr(){

  if(iniciar_conteo){
    mostrarBCD(count); //muestra el numero (inicia en 10)
    count--; //Se decrementa conteo regresivo
    if(count==-1){
      Timer1.stop(); //Para el timer y le quita el interruptar
      Timer1.detachInterrupt();
      start=false; // termina el juego
      apagar7seg(); //apago pantalla
    }; //para el conteo cuando llega a cero
  }else{
    mostrarBCD(precount);
    //fSerial.println(precount);
    precount--;
    if(precount<0){
      Serial.println("Go!");
      apagar7seg(); //apaga 7 segmentos¿
      iniciar_conteo = true;
      start = true; //El juego empieza cuando preinicio termina
    }
  }
}


void loop() {
  
  if(Serial.available()>0){ //Leo si hay algo en el input del serial
    iniciar_conteo = false; //todavia no se inicia el conteo
    precount = 3;
    apagarTodos(); //apago juego anterior
    countright = 0; //reset contadores
    countleft = 0;
    count = Serial.parseInt(); 
    Timer1.initialize(); //inicializo timer
    Timer1.attachInterrupt( timerIsr ); //adjunto interruptor
  }

  if(start){
      //Serial.println(digitalRead(BOTON1));
      // read the state of the switch into a local variable:
      int reading = digitalRead(BOTON1);
      
      // check to see if you just pressed the button
      // (i.e. the input went from LOW to HIGH),  and you've waited
      // long enough since the last press to ignore any noise:
      
      // If the switch changed, due to noise or pressing:
      if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
      }
      
      if ((millis() - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
      
        // if the button state has changed:
        if (reading != buttonState) {
          buttonState = reading;
          // only toggle the LED if the new button state is HIGH
          if (buttonState == HIGH) {
            countright++;
            Serial.print("Contador derecho ");Serial.println(countright);
          }
        }
        
        
      }
      
      // set the LED:
      //Serial.println(ledState);
      //digitalWrite(ledPin, ledState);
      
      // save the reading.  Next time through the loop,
      // it'll be the lastButtonState:
      lastButtonState = reading;
      
      // read the state of the switch into a local variable:
      int reading1 = digitalRead(BOTON2);
      
      // check to see if you just pressed the button
      // (i.e. the input went from LOW to HIGH),  and you've waited
      // long enough since the last press to ignore any noise:
      
      // If the switch changed, due to noise or pressing:
      if (reading1 != lastButtonState1) {
        // reset the debouncing timer
        lastDebounceTime = millis();
      }
      
      if ((millis() - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
      
        // if the button state has changed:
        if (reading1 != buttonState1) {
          buttonState1 = reading1;
          
          // only toggle the LED if the new button state is HIGH
          if (buttonState1 == HIGH) {
            countleft++;
            Serial.print("Contador izquierdo ");Serial.println(countleft);
          }
        }
      
       
        //Serial.print("Contador izquierdo ");Serial.println(countleft++);
      }
      
      // set the LED:
      
      //Serial.println(ledState1);
      // digitalWrite(ledPin1, ledState1);
      
      // save the reading.  Next time through the loop,
      // it'll be the lastButtonState:
      lastButtonState1 = reading1;
      rank();
  }
  
}

void rank(){
 if(countleft==countright){
    encender(tablero[0]);
    //Serial.println("empate");
  }else if(countleft>countright){
    int offset = countleft-countright;
    if(offset<4){
      encender(tablero[3]);
      //Serial.println(3);
    }else if(offset<6){
      encender(tablero[2]);
      //Serial.println(2);
    }else{
      encender(tablero[1]);
      //Serial.println(1);
    }
  }else{
    int offset = countright-countleft;
    if(offset<4){
      encender(tablero[4]);
      //Serial.println(4);
    }else if(offset<6){
      encender(tablero[5]);
      //Serial.println(5);
    }else{
      encender(tablero[6]);
      //Serial.println(6);
    }
  }
}


/**
 * Apaga todos los led de array tablero
 */
void apagarTodos(){
  for(int i=0;i<TABLERO_SIZE;i++){
    apagar(tablero[i]);
  }
}

/*
 * Convierte de bcd a decimal - takes 1384 usec for 1000 calls
 */
uint8_t bcd2dec2(uint8_t n){return n - 6 * (n/16);}

/**
 * Convierte de decimal a bcd - takes 2244 usec for 1000 calls
 */
uint8_t dec2bcd2(uint8_t n){
  uint16_t a = n;
  byte b = (a*103) >> 10;  // this equals:  b = a/10; 
  uint8_t res = n + b*6;
  //Serial.print("decimal ");Serial.print(n);Serial.print(" a bcd ");Serial.println(res);
  return  n + b*6;
}

/**
 * Toma numero decimal b y lo muestra en el 7 segmentos activando las señales
 * correspondiente
 */
void mostrarBCD(uint8_t b){
  int c = dec2bcd2(b);
  for(int i = 0;i<ABCD_SIZE;i++){
    digitalWrite(abcd[i], bitRead(c,i));
  }
}

void apagar7seg(){
  for(int i = 0;i<ABCD_SIZE;i++){
    digitalWrite(abcd[i],1);
  }
}

/**
 * Enciende pin y apaga todos los demas pins del tablero
 */
void encender(int pin){
  for(int i=0;i<TABLERO_SIZE;i++){
    if(tablero[i]!=pin){
      apagar(tablero[i]);
    }
  } 
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(tiempo); 
}
void apagar(int pin){
   digitalWrite(pin, LOW); 
}



/**
 * Metodo para probar funcionamento del 7 segmentos, imprime dig 0..9
 */
void prueba7seg(){
  for(uint8_t i=0;i<10;i++){
    //Serial.print("muestra ");Serial.println(i);
    mostrarBCD(i);
    delay(600);
  }
}

/**
 * prueba de mostrarbcd
 * mostrarBCD(0);
  delay(600);
  mostrarBCD(1);
  delay(600);
  mostrarBCD(2);
  delay(600);
  mostrarBCD(3);
  delay(600);
  mostrarBCD(4);
  delay(600);
  mostrarBCD(5);
  delay(600);
  mostrarBCD(6);
  delay(600);
  mostrarBCD(7);
  delay(600);
  mostrarBCD(8);
  delay(600);
  mostrarBCD(9);
  delay(600);*
 */

 /**
  *  value = digitalRead(BOTON1);  //lectura digital de pin
 
  //mandar mensaje a puerto serie en función del valor leido
  if (value == HIGH) {
      Serial.println("Encendido");
  }
  else {
      Serial.println("Apagado");
  }
  delay(1000);*/



