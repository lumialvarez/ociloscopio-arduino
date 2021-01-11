/*
    Osciloscopio.ino
    Autor: Luis Miguel Alvarez
    Fecha: 26/04/2014
    Osciloscopio digital Arduino, permite la medicion de 
    las senales electricas en el tiempo con un rango de operacion
    de 0 a 5 Voltios (Capaz de soportar hasta 20).
    
    Probado con Arduino IDE 1.6.2, tarjetas Arduino MEGA y Leonardo.
*/

// Librerias Requeridas
#include <Adafruit_GFX.h>    // Libreria Graficos
#include <SWTFT.h> // Libreria para la pantalla especifica


// Definicion de los colores en hexadecimal
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Definicion del Objeto que gestiona el funcionamiento de la pantalla
SWTFT tft;

// Maxima cantidad de datos que se muestran en la pantalla (Un barrido completo)
const int n_datos_lectura = 160;

// Estructura donde se almacenan los datos obtenidos
int datos[n_datos_lectura];

// Pin donde se conecta la interfaz electronica
const int pin_captura_datos = A5;

// Cantidad de pixeles que se se usan para la grafica de la senal
const int altura_graficos = 178;
int paso = 2;

// variables auxiliares
unsigned long t_muestra = 0;
unsigned long t_total = 0;
unsigned long n_muestras = 0;
unsigned long tt;

/* 
   Este metodo se encarga de inicializar variables, objetos y 
   la presentacion de inicial del dispositivo.
   Adicionalmente se envian datos por el Serial (El ID, ancho 
   y alto de la pantalla) a modo informativo.
*/
void setup() {
  Serial.begin(9600);

  tft.reset();
  uint16_t identifier = tft.readID();
  Serial.print("LCD driver chip: ");
  Serial.println(identifier, HEX);
  tft.begin(identifier);
  Serial.print("width: "); Serial.println(tft.width());
  Serial.print("height: "); Serial.println(tft.height());
  tft.setRotation(3);
  graficar_presentacion_inicial();
  graficar_adornos();
  graficar_informacion();
}

/* 
   Este metodo principal que se repite "infinitamente", 
   se encarga de la captura de los datos y posteriormente llama al 
   metodo que grafica el dato obtenido.
   
   Cada vez que se obtiene los datos de un barrido completo 
   (se llena la pantalla) osea se obtienen 'n_datos_lectura' datos
   se calcula y se muestra informacion adicional 
   (Tiempo promedio entre muestras y fecuencia del muestreo)
*/
void loop() {
  int static i = 0;
  int val;
  val = analogRead(pin_captura_datos);
  datos[i] = map(val, 0, 1023, 0, altura_graficos - 4);
  graficar_datos_pantalla(i);
  i++;
  if (i >= n_datos_lectura) {
    i = 0;
    graficar_informacion();
  }
}

/* 
   Este metodo muestra graficamente la magnitud del dato de 
   entrada en la pantalla. Esto funciona al borrar la magnitud 
   graficada anteior y grafica el nuevo valor.
  
   @param magnitud Entero que representa la magnitud de la 
   senal a graficar (entre 0 y 'altura_graficos').
*/
void graficar_datos_pantalla(int magnitud) {
  // Borrado del grafico anterior
  for (int k = 0 ; k < paso ; k++) {
    tft.drawFastVLine((magnitud * paso) + k, 30, altura_graficos - 1, BLACK); 
    //Borrado del grafico anterior con una linea
    tft.drawPixel((magnitud * paso) + k,
    (altura_graficos/2) + 30,CYAN);  
    //Pixel para formar la linea central
  }
  //Graficacion de la senal
  if (magnitud < 1) {
    magnitud = 1;
  }
  tft.drawLine(magnitud * paso, altura_graficos - datos[magnitud] + 26, (magnitud - 1)*paso, altura_graficos - datos[magnitud - 1] + 29, GREEN);
}


/* 
   Este metodo muestra graficamente una presentacion que 
   se muestra al inicio de la opeacion del dispositivo.
*/
void graficar_presentacion_inicial() {
  tft.fillScreen(BLACK);
  tft.setCursor(10, 50);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println("Osciloscopio V2.0");
  tft.setCursor(50, 90);
  tft.setCursor(30, 180);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("By:  Luis Miguel Alvarez A.");
  tft.setCursor(60, 195);
  tft.println("Yulian Andres Zapata M.");

  for (int i = 0 ; i < 320 ; i++) {
    tft.fillRect(i, 238, 2, 1, WHITE);
    delay(10);
  }
  delay(100);
  tft.fillScreen(BLACK);
}

/* 
   Este metodo muestra graficamente las marcas de 
   referencia y etiquetas que van a quedar estaticas 
   una vez que se inicie
   la operaciond del dispositivo.
*/
void graficar_adornos() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.setCursor(80, 0);
  tft.print("Osciloscopio V2.0");
  delay(300);
  tft.setCursor(0, 220);
  tft.print("Tiempo entre muestras:   ms");
  tft.setCursor(0, 230);
  tft.print("Frecuencia de muestreo: ");
  tft.setCursor(220, 220);
  tft.print("Entrada [0V,5V]");
  delay(300);
  for (int i  = 0 ; i < 5 ; i++) {
    tft.drawFastVLine((320 / 4)*i, 208, 3, WHITE);
    tft.drawFastVLine((320 / 4)*i, 27, 3, WHITE);
  }
  tft.drawFastHLine(0, (altura_graficos/2) + 30, 320, CYAN);
  tft.drawRect(-1, 29, 322, altura_graficos + 1, WHITE);
  delay(300);
}


/* 
   Este metodo muestra graficamente los valores que cambian
   durante la opeacion del dispositivo.
   Por ejemplo la velocidad promedio de muestreo y el 
   tiempo entre estas.
   
   Funciona borrando los valores anteriores 
   (reencribiendo el valor anterior con negro), 
   calculando los datos y 
   escribiendolos en la pantalla.
*/
void graficar_informacion() {
  // Borrado de datos anteriores
  tft.setTextColor(BLACK);  tft.setTextSize(1);
  tft.setCursor(132, 220);
  tft.println(t_muestra);
  tft.setCursor(140, 230);
  tft.println(n_muestras);
  // Calculo de los nuevos valores 
  //(tiempo actual menos el tiempo referencia anterior)
  t_total = micros() - tt;
  t_muestra = (t_total / n_datos_lectura) / 1000;
  n_muestras = 1000 / t_muestra;
  // Captura del tiempo de referencia
  tt = micros();
  // Escritura de los nuevos valores
  tft.setTextColor(WHITE);
  tft.setCursor(132, 220);
  tft.println(t_muestra);
  tft.setCursor(140, 230);
  tft.println(n_muestras);
}
