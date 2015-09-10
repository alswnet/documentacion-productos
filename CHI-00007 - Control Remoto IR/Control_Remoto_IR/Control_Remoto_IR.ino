//Se incluye la libreria de descodificacion del control
#include <IRremote.h>

//Pin usado para el receptor, puede ser cualquier pin que pueda
//generar interrupciones por cambio de nivel (en el Arduino Uno
//todos los pines se permiten)
int pinReceptor = 11;

//Se declara la clase del receptor y la estructura con los datos
//descodificados
IRrecv receptor(pinReceptor);
decode_results resultado;

//Cantidad de teclas del control
const int totalTeclas = 21;

//Tabla de codigos de las teclas de control, en orden
//correlativo con la tabla de nombres
long int tablaCodigosNEC[totalTeclas] {
  0xFFA25D, 0xFF629D, 0xFFE21D,
  0xFF22DD, 0xFF02FD, 0xFFC23D,
  0xFFE01F, 0xFFA857, 0xFF906F,
  0xFF6897, 0xFF9867, 0xFFB04F,
  0xFF30CF, 0xFF18E7, 0xFF7A85,
  0xFF10EF, 0xFF38C7, 0xFF5AA5,
  0xFF42BD, 0xFF4AB5, 0xFF52AD,
};

//Tabla de nombres de las teclas de control, en orden
//correlativo con la tabla de codigos
char *nombreTecla[totalTeclas] {
  "CH-", "CH", "CH+",
  "PREV", "NEXT", "PLAY",
  "VOL-", "VOL+", "EQ",
  "0", "100+", "200+",
  "1", "2", "3",
  "4", "5", "6",
  "7", "8", "9",
};

void setup() {
  //Inicializa la terminal
  Serial.begin(9600);
  //Inicializa el receptor
  receptor.enableIRIn();
}

void loop() {
  //Verifica si el receptor descodifico algo
  if (receptor.decode(&resultado)) {
    //Se asegura que el resultado sea un codigo NEC (si no se
    //hace, se recibe mucha basura en algunos receptores)
    if (resultado.decode_type == NEC) {
      //Se busca el codigo de la tecla, a ver si existe en la
      //tabla (Si no existe fue ruido y se descarta)
      for (int i = 0; i < totalTeclas; i++) {
        if (resultado.value == tablaCodigosNEC[i]) {
          //Si el codigo existe en la tabla, se imprime la
          //informacion de la tecla
          Serial.print("Codigo de la tecla: ");
          Serial.print(resultado.value, HEX);
          Serial.print(" - ");
          Serial.println(nombreTecla[i]);

          //Como se encontro un codigo, se termina la busqueda
          break;
        }
      }
    }

    //Se le indica al receptor que siga identificando codigos
    receptor.resume();
  }
}
