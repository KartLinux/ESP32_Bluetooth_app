
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

//variables para interpolación
float targetServoX = 90.0; // Meta interpolada para el servo X
float targetServoY = 90.0; // Meta interpolada para el servo Y
const float servoSpeed = 2.0; // Velocidad máxima de movimiento en grados por actualización

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
    //Enviar datos simulados de sensor cada intervalo
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();
    int sensorData = random(20, 50); // Simula datos entre 20 y 50
    SerialBT.print("SensorData: ");  // Enviar prefijo para identificar
    SerialBT.println(sensorData);
    Serial.print("Enviando datos del sensor: ");
    Serial.println(sensorData);
  }
  //--------------------------------------------------------
  // Llamar a la nueva función para mover los servos suavemente
  moveServoSmoothly();
  // Pausa para evitar saturar el monitor serie
  delay(20); // Intervalo reducido para movimiento más suave
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

    // Comandos básicos para controlar el LED
    if (data == "LED_ON") {
        toggleLed(true);
        SerialBT.println("LED encendido");
    } 
    else if (data == "LED_OFF") {
        toggleLed(false);
        SerialBT.println("LED apagado");
    } 

  // El formato del comando esperado: "SERVO:X,Y"
  if (data.startsWith("SERVO:")) {
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
