#include <TimerOne.h>

int DA_MULT = 4;
int W_S_MULT = 5;
long DIT_TIME = 60000; //microseg 1
long DASH_TIME = DIT_TIME*DA_MULT;
long WORD_SPACE_TIME = DIT_TIME*W_S_MULT;
long UMBRAL = 40000;

String uno = "1";
String cero = "0";
String di = uno;
String da = di+di+di+di;
String s_ = cero;
String l_ = cero+cero+cero+cero;
String w_ = cero+cero+cero+cero+cero;

char alfa[] = {' ','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0'};
String mors[] = {w_,
di+s_+da,
da+s_+di+s_+di+s_+di,
da+s_+di+s_+da+di,
da+s_+di+s_+di,
di,
di+s_+di+s_+da+s_+di,
da+s_+da+s_+di,
di+s_+di+s_+di+s_+di,
di+s_+di,
di+s_+da+s_+da+s_+da,
da+s_+di+s_+da,
di+s_+da+s_+di+s_+di,
da+s_+da,
da+s_+di,
da+s_+da+s_+da,
di+s_+da+s_+da+s_+di,
da+s_+da+s_+di+s_+da,
di+s_+da+s_+di,
di+s_+di+s_+di,
da,
di+s_+di+s_+da,
di+s_+di+s_+di+s_+da,
di+s_+da+s_+da,
da+s_+di+s_+di+s_+da,
da+s_+di+s_+da+s_+da,
da+s_+da+s_+di+s_+di,
di+s_+da+s_+da+s_+da+s_+da,
di+s_+di+s_+da+s_+da+s_+da,
di+s_+di+s_+di+s_+da+s_+da,
di+s_+di+s_+di+s_+di+s_+da,
di+s_+di+s_+di+s_+di+s_+di,
da+s_+di+s_+di+s_+di+s_+di,
da+s_+da+s_+di+s_+di+s_+di,
da+s_+da+s_+da+s_+di+s_+di,
da+s_+da+s_+da+s_+da+s_+di,
da+s_+da+s_+da+s_+da+s_+da};

String inputString = "";
boolean stringComplete = false;
boolean primeraVez = true;
unsigned int timer;
String cadenaMors = "";
String palabra;
boolean traducida = false;
boolean esEspacio = false;
byte activador = 5;
byte receptor_morse = 2;
byte receptor_act = 3;
byte transmisor = 4;
boolean isInactive = false;
boolean canReceive = false;
boolean canSend = false;

const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
String palabraMorse;
boolean newData = false;
byte WORD_SIZE;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //Configurar el serial
  pinMode(receptor_morse,INPUT_PULLUP);
  pinMode(receptor_act,INPUT_PULLUP);
  pinMode(transmisor,OUTPUT);
  pinMode(activador,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(receptor_morse),leerCambioLuz,CHANGE);
  attachInterrupt(digitalPinToInterrupt(receptor_act),iniciar_io,CHANGE);
  Timer1.initialize(5000000);
  //Timer1.attachInterrupt( timerIsr ); // attach the service routine her
  //TESTING PURPOSES
  //delay(5000);
  
}

/**Convierte string str a char* */
char* toCharray(String str){
  int l = str.length();
  char *c = (char *) malloc(l);
  str.toCharArray(c,l+1);
  return c;
}

/** Maneja señal de transmisión */
void iniciar_io(){
  if(isInactive){
    Serial.println("termina transmisión");
    if(cadenaMors.length()>0){
      Serial.println(cadenaMors);
      int len = cadenaMors.length();
      int count =0;
      int j = 0;
      String let;
      for(int i=0;i<len;i++){
        //Serial.print("indice i ");Serial.println(i);
        if(cadenaMors.charAt(i)=='0'){
          count++;
          // Serial.print("0 encontrado, contador en ");Serial.println(count);
        }else{
          count=0;
        }
        if(count==W_S_MULT){
          Serial.print("j en ");Serial.println(j);
          let = cadenaMors.substring(j,i-(W_S_MULT-1));
          //Serial.print("j en ");Serial.println(j);
          //Serial.print("palabra recuperada ");Serial.println(let);
          traducir(let);
          palabra+=" ";
          j=i+1;
          count=0;
        }else if(count==DA_MULT && cadenaMors.charAt(i+1)!='0'){
          let = cadenaMors.substring(j,i-(DA_MULT-1));
          //Serial.print("letra recuperada ");Serial.println(let);
          traducir(let);
          j=i+1;
          count=0;
        }
      }
      let = cadenaMors.substring(j,cadenaMors.length());
      Serial.print("ultima letra recuperada ");Serial.println(let);
      traducir(let);
    }
    isInactive = false;
    canReceive = false;
    
  }else{
    //va a empezar a transmitir/recibir
    Serial.println("inicia transmisión");
    canReceive = true;
    isInactive = true;
                
  }
}

void leerCambioLuz(){
  if(canReceive){
    if(primeraVez){
      //limpiar para leer nueva cadena
      //Serial.println(Timer1.read());
      Timer1.restart();
      //String cadena = new String();
    }else{
      //Serial.println(contador);
      long timer = Timer1.read();
      //Serial.println(timer);
      
      if(timer>(DIT_TIME-UMBRAL) && timer<(DIT_TIME+UMBRAL)){
        if (esEspacio){
          cadenaMors += s_;
          esEspacio = false;
        }else{
          cadenaMors += di;
          esEspacio = true;
        }
        //Serial.println("cadena= " + cadena);
      }else if(timer>DASH_TIME-UMBRAL && timer <DASH_TIME+UMBRAL){
        if (esEspacio){
          cadenaMors += l_; 
          esEspacio = false;       
        }else{
         cadenaMors += da; 
         esEspacio = true;
        }
        //Serial.println("cadena= " + cadena);
      } else if(timer>WORD_SPACE_TIME-UMBRAL && timer < WORD_SPACE_TIME+UMBRAL){
        //es espacio de cadena
        cadenaMors += w_;
        esEspacio = false;
      }
      
      Timer1.restart();
    }
    primeraVez = false;
    //Serial.println(cadenaMors);
  }
}

void traducir(String letra){
  for(int i = 0;i<sizeof(mors);i++){
    if(letra == mors[i]){
      //Serial.print("letra traducida ");
      palabra += alfa[i];
      Serial.println(palabra);
    }
  }
  traducida = true;
}

void transmitir(String cadena){ //recibe cadena para trasmitir
  //Serial.println("Comienza a transmitir morse");
  encender(activador);
  delay(2000);
//Serial.println(cadena);
cadena = cadenaMors+cadena; //Toma palabra recibida y la agrega a cadena ingresada.
Serial.println("cadena completa= " + cadena);
 for(int i=0;i<cadena.length();i++){
    char p=cadena.charAt(i);
    //Serial.println(p);
    if(p=='1'){
         if(cadena.charAt(i+1)=='1'){
             //Serial.println("da");
             encender(transmisor);
             delay(DASH_TIME/1000);
             apagar(transmisor);
             i=i+3; //pasa al siguiente simbolo
         }else{
          //Serial.println("di");
          encender(transmisor);
          delay(DIT_TIME/1000);
          apagar(transmisor);
         }
    }else{
          //cuando es cero
          if(cadena.substring(i,i+W_S_MULT)==w_){
            //espacide letra
            //apagar(transmisor);
            //Serial.println("w s");
            delay(WORD_SPACE_TIME/1000);
            i=i+4; //pasa al siguiente simbolo
          }else if(cadena.substring(i,i+DA_MULT)==l_){
            //apagar(transmisor);
            //Serial.println("l s");
            delay(DASH_TIME/1000);
            i=i+3; //pasa al siguiente simbolo
          }else{
            delay(DIT_TIME/1000);
          }
    }
  }//fin for
  apagar(transmisor);
  delay(2000);
  apagar(activador);
  canSend = false;
}//fin trasm

void toMorse(char str[]){
  //Serial.print("palabra recibida");Serial.println(str);
  //Serial.print("tamano de palabra "); Serial.println(WORD_SIZE);
  for(int i = 0; i<WORD_SIZE;i++){
    char c = str[i];
    //Serial.print("caracter tomado ");Serial.println(c);
    if(c==' '){
      palabraMorse = palabraMorse.substring(0,palabraMorse.length()-DA_MULT);
      palabraMorse += codificar(c);
    }else{
      palabraMorse += codificar(c) + l_;
    }
  }
  palabraMorse = palabraMorse.substring(0,palabraMorse.length()-DA_MULT);
  Serial.print("palabra decodificada ");Serial.println(palabraMorse);
}

String codificar(char c){
  String r = "";
  for(int i = 0; i<sizeof(alfa);i++){
    if(alfa[i]==c){
      r = mors[i];
    }
  }
  //Serial.print("caracter decodificado ");Serial.println(r);
  return r;
}

void loop() {
  //Serial.print(digitalRead(receptor_act));

  /**if(canSend){
    encender(activador);
    delay(2000);
    toMorse(receivedChars);
    transmitir(palabraMorse);
  }*/

  recvWithEndMarker();
  showNewData();
  if(canSend){
    toMorse(receivedChars);
    transmitir(palabraMorse);
  }
}


void recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        //Serial.println(rc);
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            WORD_SIZE = ndx;
            ndx = 0;
            newData = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
        canSend = true;
    }
    
}

void encender(int pin){
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(tiempo); 
}
void apagar(int pin){
   digitalWrite(pin, LOW); 
}



