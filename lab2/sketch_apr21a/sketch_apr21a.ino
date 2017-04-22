// Declaramos pines a usar para 74ls48
int A = 4;
int B = 5;
int C = 6;
int D = 7;

void setup() {
  // put your setup code here, to run once:
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  Serial.begin(9600);

}

uint8_t bcd2dec2(uint8_t n)  // takes 1384 usec for 1000 calls
{
  return n - 6 * (n/16);
}

uint8_t dec2bcd2(uint8_t n)  // takes 2244 usec for 1000 calls
{
  uint16_t a = n;
  byte b = (a*103) >> 10;  // this equals:  b = a/10; 
  return  n + b*6;
}

uint8_t a = 25;

uint8_t b = dec2bcd2(a);





void loop() {
  // put your main code here, to run repeatedly:
//Serial.println(b);
//Numero 0
  digitalWrite(A, bitRead(b,0));
  digitalWrite(B, bitRead(b,1));
  digitalWrite(C, bitRead(b,2));
  digitalWrite(D, bitRead(b,3));

 delay(300);

 
//Numero 1
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);

  delay(300);
//Numero 2 
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);

  delay(300);
//Numero 3 
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);

  delay(300);
 //Numero 4
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);

  delay(300);
//Numero 5
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);

  delay(300);
//Numero 6
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);

  delay(300);
//Numero 7
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, LOW);

  delay(300);
//Numero 8
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);

  delay(300);
//Numero 9
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);

  delay(300);
//Numero 10
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);

  delay(300);
//Numero 11
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, LOW);
  digitalWrite(D, HIGH);

  delay(300);
//Numero 12
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);

  delay(300);
//Numero 13
  digitalWrite(A, HIGH);
  digitalWrite(B, LOW);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);

  delay(300);
//Numero 14
  digitalWrite(A, LOW);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);

  delay(300);
//Numero 15
  digitalWrite(A, HIGH);
  digitalWrite(B, HIGH);
  digitalWrite(C, HIGH);
  digitalWrite(D, HIGH);

}
