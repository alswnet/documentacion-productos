#include <SoftwareSerial.h>
#include <SparkFunESP8266WiFi.h>

const char MiSSID[] = "ALSW";
const char MiPSK[] = "21241324";

const int PinTemperatura = 0;

ESP8266Server MiniServer = ESP8266Server(80);

const String htmlCabeza = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/html\r\n"
                          "Connection: close\r\n\r\n"
                          "<!DOCTYPE HTML>\r\n"
                          "<html>\r\n";

void setup() {

  Serial.begin(9600);//Abrimos el puerto serial para poder manda info de la PC al arduino
  IniciarESP8266();//Funcion para activar el wifi
  ConectandoESP8266();//Se conecta al AP
  ActivarServer();//Activa el server
}

void loop() {
  ServirPaginaWeb();
}

void IniciarESP8266() {
  int prueva = esp8266.begin();//Inicia la comunicasion con el modulo, si falla devuelta false
  if (prueva != true) {
    Serial.println("Error encontrando al ESP8266");
    ErrorLoop();
  }
  Serial.println("Exitos encontrando al ESP8266");
}

void ConectandoESP8266() {
  //Existen diferentes modo que podemos activar los modos son
  //  1 - ESP8266_MODE_STA - Station only - Modo Cliente
  //  2 - ESP8266_MODE_AP - Access point only - Modo Router
  //  3 - ESP8266_MODE_STAAP - Station/AP combo - Modo Combo

  int Respuesta = esp8266.getMode();//Vemos en que modo esta
  if (Respuesta != ESP8266_MODE_STA) {//Si no esta en modo cliente lo cambiaremos
    Respuesta = esp8266.setMode(ESP8266_MODE_STA);
    if (Respuesta < 0) {//Verifica si se puedo cambiar si da menos de 0 es un error
      Serial.println("Error cambiando el modo");
      ErrorLoop();
    }
  }
  Serial.println("Activaso modo cliente");

  Respuesta = esp8266.status();//Ve si ESP8266 esta conectado 1, si devuelte 0 lo tendremos que conectar
  if (Respuesta <= 0) {
    Respuesta = esp8266.connect(MiSSID, MiPSK);//Conecta usando la contraseña y nombre de router de la red
    if (Respuesta < 0) {
      Serial.println("Error con coneccion");
      ErrorLoop();
    }
  }
  char ConectadoSSID[24];//varialbe donde se guarda a que AP esta conectado
  memset(ConectadoSSID, 0, 24);
  Respuesta = esp8266.getAP(ConectadoSSID);
  if (Respuesta < 0) {//Verifica si esta conectado
    Serial.print("Error obteniendo IP con ");
    Serial.println(ConectadoSSID);
    ErrorLoop();
  }
  //Imprimi a que AP y el IP que esta usando
  IPAddress MiIP = esp8266.localIP();
  Serial.print("Estoy conectado ");
  Serial.println(ConectadoSSID);
  Serial.print("Mi Ip es: ");
  Serial.println(MiIP);

}

void ActivarServer() {
  MiniServer.begin();
  Serial.println();
  Serial.print("MiniServo Listo entra a : ");
  Serial.println(esp8266.localIP());
  Serial.println();
}

void ServirPaginaWeb() {

  ESP8266Client Cliente = MiniServer.available(500);//Espera a un usuario a conectarse, espera 500 milisegundos
  if (Cliente) {
    Serial.println("Cliente conectado");
    boolean LineaEnBlanco = true;//Salto de line
    while (Cliente.connected()) {
      if (Cliente.available()) {
        char c = Cliente.read();
        if ( c == '\n' && LineaEnBlanco) {
          Serial.println("Enviando HTML");
          Cliente.print(htmlCabeza);//Enviamos la cabecera HTML
          String htmlEnvio;
          htmlEnvio = "<h1>Temperatura Local ALSW</h1>\n";
          htmlEnvio += "<h2>Temperatura = ";
          htmlEnvio +=  String(TempaturaCentigrados());//Agregamos la temperatura
          htmlEnvio += "C</H2>";
          htmlEnvio += "</html>\n";
          Cliente.print(htmlEnvio);//Envia al final del documento HTML
          break;
        }
        if (c == '\n') {
          LineaEnBlanco = true;
        }
        else if ( c != '\r') {
          LineaEnBlanco = false;
        }
      }
    }
    delay(3);//tiempo para que el navegado reciva la la data
    Serial.println("Cliente Desconectado");
    Cliente.stop();

  }

}

int TempaturaCentigrados() {
  //Devuelve la temperatura e grados Cengirados
  return  ((analogRead(PinTemperatura) * 0.004882814) - 0.5) * 100.0;

}

void ErrorLoop() {
  //ciclo infinito detiene el micro infinitamente al infinito o hasta que se reinicie
  for (;;)
    ;
}
