#include <SoftwareSerial.h>

#define DEBUG true

SoftwareSerial modulo(3, 2); // Pin 2 receptor, Pin 3 transmisor

void setup()
{
  Serial.begin(115200);
  modulo.begin(115200); // velocidad de transmisión podria ser diferente aunque genralmente es la que usa el el modulo modulo

  pinMode(10, OUTPUT); //configuramos los pines como salidas
  digitalWrite(10, LOW);

  pinMode(11, OUTPUT); //configuramos los pines como salidas
  digitalWrite(11, LOW);

  pinMode(12, OUTPUT); //configuramos los pines como salidas
  digitalWrite(12, LOW);

  pinMode(13, OUTPUT); //configuramos los pines como salidas
  digitalWrite(13, LOW);

  Serial.println("Inicia configuración del modulo");

  sendData("AT+RST\r\n", 2000, DEBUG); // resetea el  modulo
  sendData("AT+CWMODE=2\r\n", 1000, DEBUG); // configure el modulo con AP y Estación al mismo tiempo
  sendData("AT+CWJAP='Telematica [Clase]','Telem4tic4'\r\n", 1000, DEBUG); // cambiar estos datos por los de tu red
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // Este comandos nos indica la configuración ip que ha tomado nuestro módulo
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // Configura multiples coneciones
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // Inicializa el servidor web
}
void loop()
{
  if (modulo.available()) // Verifica si el modulo está disponible
  {
    if (modulo.find("+IPD,")) //Busca el campo de texto +IPD
    {
      delay(1000); //Espera un segundo para leer todos los datos del puerto serie
      int connectionId = modulo.read() - 48;

      modulo.find("sal="); // avanza el cursor a "sal="

      int pinNumber = (modulo.read() - 48) * 10; // conseguir primer número, si el pin es 13, entonces el primer número es 1, entonces se multiplican para obtener 10
      pinNumber += (modulo.read() - 48); // consegue el segundo número, si el número de pin es 13, entonces el segundo número es 3, a continuación, le añade al primero

      digitalWrite(pinNumber, !digitalRead(pinNumber));


      String closeCommand = "AT+CIPCLOSE=";
      closeCommand += connectionId;
      closeCommand += "\r\n";

      sendData(closeCommand, 1000, DEBUG); // cierra la conección
    }
  }
}


String sendData(String command, const int timeout, boolean debug)
{
  String cadena = "";

  Serial.print("comando a imprimir ");
  Serial.println(command);

  modulo.print(command); // enviar el carácter leído al modulo

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    //Serial.println("Paso tiempo de timeout");
    while (modulo.available())
    {

      // Muestra los Datos de salida del modulo por el seguimiento de la serie
      char c = modulo.read(); //Lee el proximo caracter
      //Serial.print("modulo leido");Serial.println(c);
      cadena += c;
    }
  }

  if (debug)
  {
    Serial.print(cadena);
  }

  return cadena;
}
