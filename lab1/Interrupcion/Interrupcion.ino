double potenciometro;
String inputString = "";
void setup() {
  // put your setup code here, to run once:
  
   pinMode(4, OUTPUT);
   pinMode(2,INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(2),myFunction,RISING);
   Serial.begin(9600);
}

void loop() {
  
  
}
void encender(int pin){
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(tiempo); 
}
void apagar(int pin){
   digitalWrite(pin, LOW); 
}

void myFunction(){
  encender(4);
}
void myFunction2(){
  apagar(4);
}


