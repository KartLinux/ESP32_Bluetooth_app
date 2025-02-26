#include "BluetoothSerial.h"  // Librería para Bluetooth Serial
#include <ESP32Servo.h> // Bliblioteca para los servomotores

// Crea objetos Servo para cada uno
Servo servoX;
Servo servoY;
// Configuración de la conexión Bluetooth
BluetoothSerial SerialBT;
const char* DEVICE_NAME = "ESP32_ROOT"; // Nombre del dispositivo Bluetooth (puedes cambiarlo)
// Configuración del LED integrado
const int LED_PIN = 2; // Pin del LED integrado en la mayoría de los ESP32
// Variables globales
bool isLedOn = false; // Estado del LED
unsigned long lastSendTime = 0; // Para controlar el intervalo de envío de datos
const unsigned long sendInterval = 5000; // Intervalo de 5 segundos para enviar datos

// Define los pines para los servos
const int SERVO_X_PIN = 5;
const int SERVO_Y_PIN = 18;
const int SERVO_X_MIN = 0;
const int SERVO_X_MAX = 180;
const int SERVO_Y_MIN = 60;   // Límite inferior para evitar tocar el suelo
const int SERVO_Y_MAX = 120;  // Límite superior vertical

// Sensores de luz (ajusta los pines según tu conexión)
const int SENSOR_TOP = 34;
const int SENSOR_BOTTOM = 35;
const int SENSOR_LEFT = 32;
const int SENSOR_RIGHT = 33;

int top = 0;
int bottom = 0;
int left = 0;
int right = 0;

// Variables globales para almacenar los valores de los servos y la hora
int currentServoX = 90; // Posición inicial (90°)
int currentServoY = 90;
String currentTime = "00:00:00"; // Hora actual
int previousServoX = 90; // Almacena el valor anterior del servo X
int previousServoY = 90; // Almacena el valor anterior del servo Y
unsigned long lastUpdateTime = 0; // Tiempo de la última actualización
const unsigned long updateInterval = 3000; // Intervalo en milisegundos (ajústalo según lo necesites)
int tempServoX = 90; // Valores temporales para el joystick
int tempServoY = 90;

// Variables de control
bool automaticMode = false;

//variables para interpolación
float targetServoX = 90.0; // Meta interpolada para el servo X
float targetServoY = 90.0; // Meta interpolada para el servo Y
const float servoSpeed = 5.0; // Velocidad máxima de movimiento en grados por actualización

// Prototipos de funciones (para una estructura más organizada y escalable)
void setupBluetooth();
void handleReceivedData(String data);
void toggleLed(bool state);
void moveServoSmoothly(); //función para mover los servos suavemente

void setup() {
  delay(3000); // Pausa de 500 ms
  // put your setup code here, to run once:
  Serial.begin(115200);  // Inicialización del puerto serie para debug
  Serial.println("Iniciando...");  // Agrega un mensaje para verificar que el código llegó aquí
  delay(3000); // Pausa de 500 ms
  pinMode(LED_PIN, OUTPUT); // Configuración del pin del LED como salida
  pinMode(SENSOR_TOP, INPUT);
  pinMode(SENSOR_BOTTOM, INPUT);
  pinMode(SENSOR_LEFT, INPUT);
  pinMode(SENSOR_RIGHT, INPUT);

  setupBluetooth(); // Configuración del Bluetooth

  // Adjunta los servos a sus pines
  // Configurar los servos
  servoX.attach(SERVO_X_PIN); // Valores típicos de PWM: 500-2400 microsegundos
  servoY.attach(SERVO_Y_PIN);
  delay(3000); // Pausa de 500 ms
  // Posicionar los servos en el centro al inicio
  Serial.println(SERVO_X_PIN);
  servoX.write(90);
  servoY.write(90);// Pone el servo 2 en la posición media (90 grados)
  Serial.println("ESP32 listo para controlar servos.");
  Serial.println(">>>ESP32 listo para recibir comandos por Bluetooth!<<<");
}

void loop() {
  // put your main code here, to run repeatedly:
  // Revisión de si hay datos recibidos por Bluetooth
    if (SerialBT.available()) {
        String receivedData = SerialBT.readStringUntil('\n'); // Lee hasta nueva línea
        Serial.print("Comando recibido: ");
        Serial.println(receivedData);

        // Procesa el comando recibido
        handleReceivedData(receivedData);
    }
  if (automaticMode) {
    followLight(); // Lógica de seguimiento de luz
  }
    //Enviar datos simulados de sensor cada intervalo
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();
    // Leer valores actuales de los sensores de luz
    top = analogRead(SENSOR_TOP);
    bottom = analogRead(SENSOR_BOTTOM);
    left = analogRead(SENSOR_LEFT);
    right = analogRead(SENSOR_RIGHT);

    // Formatear los datos como cadena
    String dataMessage = 
        "ServoX:" + String(currentServoX) + 
        ",ServoY:" + String(currentServoY) + 
        ",Top:" + String(top) + 
        ",Bottom:" + String(bottom) + 
        ",Left:" + String(left) + 
        ",Right:" + String(right);

    SerialBT.println(dataMessage);
    Serial.print("Enviando datos del sensor: ");
    Serial.println(dataMessage);
  }
  //--------------------------------------------------------
  // Llamar a la nueva función para mover los servos suavemente
  moveServoSmoothly();
  // Pausa para evitar saturar el monitor serie
  delay(20); // Intervalo reducido para movimiento más suave
}
// Función para seguir la fuente de luz
// =============================================
// FUNCIÓN PRINCIPAL DE SEGUIMIENTO
// =============================================
void followLight() {
  // Zonas activas (ajustar según tus sensores)
  const int umbralOscuro = 2500; // Valor alto = sombra
  const int umbralClaro  = 1500; // Valor bajo = luz

  // Definición de posiciones para el servo Y:
  // Cuando el TOP ve sombra, se mueve completamente (posición 0°).
  // Cuando el BOTTOM ve sombra, se mueve sólo hasta un límite (posición 60°).
  const int posCompletaTop     = 0;   // Movimiento completo hacia el lado opuesto al TOP
  const int posLimitadaBottom  = 60;  // Límite cuando el BOTTOM tiene sombra

  // Leer sensores
  top    = analogRead(SENSOR_TOP);
  bottom = analogRead(SENSOR_BOTTOM);
  right  = analogRead(SENSOR_RIGHT);
  left   = analogRead(SENSOR_LEFT);

  // Lógica para el eje Y (vertical):
  if (top > umbralOscuro && bottom < umbralClaro) {
    // Caso: TOP ve sombra y BOTTOM luz.
    // Se mueve completamente hacia el lado opuesto (0°).
    targetServoY = posCompletaTop;
  } else if (top < umbralClaro && bottom > umbralOscuro) {
    // Caso: BOTTOM ve sombra y TOP luz.
    // Se mueve solo hasta el límite (60°).
    targetServoY = posLimitadaBottom;
  } else {
    // Caso intermedio: se calcula un ángulo entre 0° y 60°.
    // Cuando la diferencia (top - bottom) es alta (más sombra en TOP),
    // se acerca a 0°; cuando es baja (más sombra en BOTTOM),
    // se acerca a 60°.
    targetServoY = map(top - bottom, -1023, 1023, posLimitadaBottom, posCompletaTop);
    targetServoY = constrain(targetServoY, posCompletaTop, posLimitadaBottom);
  }

  // Lógica para el eje X (horizontal):
  // Se mueve en dirección opuesta a la sombra:
  if (right > umbralOscuro && left < umbralClaro) {
    targetServoX = 175;
  } else if (left > umbralOscuro && right < umbralClaro) {
    targetServoX = 5;
  } else {
    targetServoX = map(left - right, -1023, 1023, 180, 0);
    targetServoX = constrain(targetServoX, 0, 180);
  }
}


// Nueva función para mover los servos suavemente
void moveServoSmoothly() {
  // Interpolación gradual hacia las metas de los servos
  if (abs(targetServoX - currentServoX) > 0.5) {
    if (targetServoX > currentServoX) {
      currentServoX += servoSpeed;
      if (currentServoX > targetServoX) currentServoX = targetServoX;
    } else {
      currentServoX -= servoSpeed;
      if (currentServoX < targetServoX) currentServoX = targetServoX;
    }
    servoX.write(currentServoX);
    Serial.print("Servo X moviéndose a: ");
    Serial.println(currentServoX);
  }

  if (abs(targetServoY - currentServoY) > 0.5) {
    if (targetServoY > currentServoY) {
      currentServoY += servoSpeed;
      if (currentServoY > targetServoY) currentServoY = targetServoY;
    } else {
      currentServoY -= servoSpeed;
      if (currentServoY < targetServoY) currentServoY = targetServoY;
    }
    servoY.write(currentServoY);
    Serial.print("Servo Y moviéndose a: ");
    Serial.println(currentServoY);
  }
}
// Función para configurar Bluetooth
void setupBluetooth() {
  Serial.println("Iniciando Bluetooth...");
    if (!SerialBT.begin(DEVICE_NAME)) {
        Serial.println("Error al iniciar Bluetooth");
        while (1); // Si hay error, se queda en un bucle infinito
    }
    Serial.print("Bluetooth iniciado como: ");
    Serial.println(DEVICE_NAME);
}
// Función para manejar los datos recibidos por Bluetooth
void handleReceivedData(String data) {
    data.trim(); // Elimina espacios y saltos de línea extra

    if (data.startsWith("AUTO")) {
      automaticMode = true;
      SerialBT.println("Modo automático activado");
    }
    else if (data.startsWith("MANUAL")) {
      automaticMode = false;
      SerialBT.println("Modo manual activado");
    }
    // Comandos básicos para controlar el LED
    else if (data == "LED_ON") {
        toggleLed(true);
        SerialBT.println("LED encendido");
    } 
    else if (data == "LED_OFF") {
        toggleLed(false);
        SerialBT.println("LED apagado");
    } 

  // El formato del comando esperado: "SERVO:X,Y"
  if (data.startsWith("SERVO:")) {
    if (automaticMode) {
      SerialBT.println("Error: En modo automático. Usa 'manual' primero.");
      return;
    }
    // Extraer las posiciones X e Y del comando
    int commaIndex = data.indexOf(',');
    if (commaIndex != -1) {
      String xValue = data.substring(6, commaIndex);
      String yValue = data.substring(commaIndex + 1);

      // Convertir las posiciones a enteros
      int newServoX = xValue.toInt();
      int newServoY = yValue.toInt();

      // Validar los rangos (0° a 180°)
        if (newServoX >= 0 && newServoX <= 180) {
            tempServoX = newServoX; // Almacena temporalmente el valor
            targetServoX = newServoX; // Actualiza la meta interpolada
        }
        if (newServoY >= 0 && newServoY <= 180) {
            tempServoY = newServoY; // Almacena temporalmente el valor
            targetServoY = newServoY; // Actualiza la meta interpolada
        }
    }
  }
    int timeIndex = data.indexOf("TIME:");
    // Procesar TIME
    if (timeIndex != -1) {
        String timeData = data.substring(timeIndex + 5);
        timeData.trim(); // Limpia directamente
        currentTime = timeData; // Actualiza la variable global
        Serial.print("Esp32: Hora recibida: ");
        Serial.println(currentTime);
    }
}
// Función para encender o apagar el LED
void toggleLed(bool state) {
    digitalWrite(LED_PIN, state ? HIGH : LOW);
    isLedOn = state;
    Serial.print("LED ");
    Serial.println(state ? "encendido" : "apagado");
}
// Función adicional para detener el Bluetooth (si lo necesitas)
void stopBluetooth() {
    SerialBT.end();
    Serial.println("Bluetooth detenido");
} 