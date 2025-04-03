#include "Wire.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "Keypad.h"

// Pantalla OLED con dirección TWI
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(-1); // -1 = no reset pin

// Configuración del sensor de alcohol
int valor_limite = 500; // Fija el valor límite en el que se activa la alarma
float valor_alcohol;
const int pinNivelAlto = 8;
const int pinNivelMedio = 9;
const int pinNivelBajo = 10;
const int pinNivelMuyBajo = 11;
const int pinBuzzer = 13;  // Pin del buzzer

// Variables para el estado de la alarma
bool alarmaActivada = false;

// Configuración del teclado 4x4
const byte FILAS = 4;
const byte COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte pinesFilas[FILAS] = {A1, A2, A3, A4}; // Conecta a los pines de las filas
byte pinesColumnas[COLUMNAS] = {2, 3, 4, 5}; // Conecta a los pines de las columnas

Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// Contraseña para desactivar la alarma
const char CONTRASENA[8] = "3121040"; // Contraseña de 7 dígitos
char contraseniaIngresada[8]; // Almacena la contraseña ingresada
byte indexContrasenia = 0;

// Función para convertir el valor analógico a g/L
float convertirAGramosPorLitro(float valorAnalogico) {
  // Esta función debe ser calibrada según tu sensor específico
  // Esta es una aproximación lineal simple
  return (valorAnalogico / 1000.0) * 5.0;  // Escala de 0 a 5 g/L aproximadamente
}

// Función para obtener un mensaje basado en el nivel de alcohol
String obtenerMensajeAlcohol(float nivelAlcohol) {
  if (nivelAlcohol < 0.2) {
    return "Sin riesgo";
  } else if (nivelAlcohol < 0.5) {
    return "Efectos leves";
  } else if (nivelAlcohol < 0.8) {
    return "Limite legal";
  } else if (nivelAlcohol < 1.0) {
    return "Alteraciones";
  } else if (nivelAlcohol < 3.0) {
    return "Deterioro severo";
  } else if (nivelAlcohol < 5.0) {
    return "Riesgo de coma";
  } else {
    return "Riesgo de muerte";
  }
}

void setup() {
  // Inicializa y borra la pantalla OLED
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
  
  // Muestra una línea de texto
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Inicializa el puerto serial
  Serial.begin(9600);
  
  // Configura los pines como salidas
  pinMode(pinNivelAlto, OUTPUT);
  pinMode(pinNivelMedio, OUTPUT);
  pinMode(pinNivelBajo, OUTPUT);
  pinMode(pinNivelMuyBajo, OUTPUT);
  pinMode(pinBuzzer, OUTPUT); // Configura el Pin 13 como salida para el Zumbador
  
  // Inicializar contraseña
  memset(contraseniaIngresada, 0, sizeof(contraseniaIngresada));
}

void loop() {
  // Lectura del sensor de alcohol
  valor_alcohol = analogRead(A0);
  float alcoholGrL = convertirAGramosPorLitro(valor_alcohol);
  String mensajeEstado = obtenerMensajeAlcohol(alcoholGrL);
  
  // Muestra la información en la pantalla OLED
  display.clearDisplay();
  
  // Mostrar el nivel de alcohol en sangre
  display.setCursor(1, 0);
  display.print("Nivel: ");
  display.print(alcoholGrL, 2);
  display.print(" g/L");
  
  display.setCursor(1, 10);
  display.print(mensajeEstado);
  
  // Si la alarma está activa, mostrar el código ingresado
  if (alarmaActivada) {
    //display.setCursor(1, 20);
    //display.print("ALARMA ACTIVA!");
    display.setCursor(1, 20);
    // Mostrar los dígitos ingresados
    for (byte i = 0; i < indexContrasenia; i++) {
      display.print(contraseniaIngresada[i]);
    }
  }
  
  display.display();
  
  // Información por Serial Monitor
  Serial.print("Alcohol: ");
  Serial.print(valor_alcohol);
  Serial.print(" | g/L: ");
  Serial.print(alcoholGrL, 2);
  Serial.print(" | Estado: ");
  Serial.println(mensajeEstado);
  
  // Control de los LEDs en función del valor de alcohol
  if (alcoholGrL >= 0.5) {  // Límite legal (0.5 g/L)
    digitalWrite(pinNivelAlto, HIGH);
    digitalWrite(pinNivelMedio, LOW);
    digitalWrite(pinNivelBajo, LOW);
    digitalWrite(pinNivelMuyBajo, LOW);
    alarmaActivada = true;  // Activa la alarma cuando se supera el límite
  } else if (alcoholGrL >= 0.3) {  // Efectos moderados
    digitalWrite(pinNivelAlto, LOW);
    digitalWrite(pinNivelMedio, HIGH);
    digitalWrite(pinNivelBajo, LOW);
    digitalWrite(pinNivelMuyBajo, LOW);
    // No se desactiva la alarma aquí
  } else if (alcoholGrL >= 0.2) {  // Efectos leves
    digitalWrite(pinNivelAlto, LOW);
    digitalWrite(pinNivelMedio, LOW);
    digitalWrite(pinNivelBajo, HIGH);
    digitalWrite(pinNivelMuyBajo, LOW);
    // No se desactiva la alarma aquí
  } else if (alcoholGrL < 0.2) {  // Sin efectos
    digitalWrite(pinNivelAlto, LOW);
    digitalWrite(pinNivelMedio, LOW);
    digitalWrite(pinNivelBajo, LOW);
    digitalWrite(pinNivelMuyBajo, HIGH);
    // No se desactiva la alarma aquí
  } else {
    digitalWrite(pinNivelAlto, LOW);
    digitalWrite(pinNivelMedio, LOW);
    digitalWrite(pinNivelBajo, LOW);
    digitalWrite(pinNivelMuyBajo, LOW);
    // No se desactiva la alarma aquí
  }
  
  // Mantener el buzzer encendido si la alarma está activada
  if (alarmaActivada) {
    digitalWrite(pinBuzzer, HIGH);
  } else {
    digitalWrite(pinBuzzer, LOW);
  }
  
  // Verificar si se presiona una tecla
  char tecla = teclado.getKey();
  if (tecla) {
    // Mostrar la tecla presionada en el Serial Monitor
    Serial.print("Tecla presionada: ");
    Serial.println(tecla);
    
    if (tecla == '#') {
      // Tecla # para verificar la contraseña
      if (strcmp(contraseniaIngresada, CONTRASENA) == 0) {
        // Contraseña correcta, desactivar la alarma
        alarmaActivada = false;
        indexContrasenia = 0;
        memset(contraseniaIngresada, 0, sizeof(contraseniaIngresada));
        Serial.println("Alarma desactivada!");
      } else {
        // Contraseña incorrecta, resetear el ingreso
        indexContrasenia = 0;
        memset(contraseniaIngresada, 0, sizeof(contraseniaIngresada));
        Serial.println("Contraseña incorrecta!");
      }
    } else if (tecla == '*') {
      // Tecla * para borrar la entrada
      if (indexContrasenia > 0) {
        indexContrasenia--;
        contraseniaIngresada[indexContrasenia] = 0;
      }
    } else if (indexContrasenia < 7) {
      // Agregar dígito a la contraseña
      contraseniaIngresada[indexContrasenia] = tecla;
      indexContrasenia++;
      contraseniaIngresada[indexContrasenia] = 0; // Terminador nulo
    }
  }
  
  delay(100); // Espera 100ms para realizar la próxima medida
}
