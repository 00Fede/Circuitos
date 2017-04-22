
char uno = 1;
char cero = 0;
char di[] = {uno};
char da[] = {di[0],di[0],di[0]};
char s_[] = {cero};
char l_[] = {cero,cero,cero};
char w_[] = {cero,cero,cero,cero,cero};

char alfa[] = 
{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','1','2','3','4','5','6','7','8','9','0'};


String inputString = "";
boolean stringComplete = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //Configurar el serial
   pinMode(2,INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(2),myFunction,CHANGE);

    Serial.println("di " + di[0]);
    Serial.println("da " + da[0]);
    for(int i = 0:sizeof(alfa)){
      Serial.println("alf " + alfa[i]);
    }
  
   
}

void myFunction(){
  encender(4);
}

void loop() {
  //Serial.println(analogRead(A5));
}

void serialEvent() {

}
void encender(int pin){
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  //delay(tiempo); 
}
void apagar(int pin){
   digitalWrite(pin, LOW); 
}


