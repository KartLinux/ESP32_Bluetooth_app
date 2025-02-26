<h1 style="font-size: 28px; font-weight: bold; color: #ff4500; text-align: center; text-transform: uppercase; letter-spacing: 2px;">
  🤖 Proyecto ESP32 con Control Bluetooth y Seguimiento de Luz ⚡
</h1>


| Arduino | Esp32 | Bluetooth |
|----------|----------|----------|
| <img src="https://github.com/user-attachments/assets/b9757e45-164a-4d03-bfc4-db7b6d6a8390" width="200" /> | <img src="https://github.com/user-attachments/assets/031d5a5d-5a21-4aef-8764-f682447fddfb" width="200" /> | <img src="https://github.com/user-attachments/assets/2a0566a4-8e0b-41d8-92c9-6fb1f9970d22" width="200" /> |


📌 Descripción del Proyecto
Este proyecto utiliza un ESP32 para controlar dos servomotores y un LED mediante una conexión Bluetooth. Además, implementa un sistema de seguimiento de luz que ajusta automáticamente los servomotores según la intensidad de luz detectada por sensores. El proyecto es ideal para aplicaciones como sistemas de seguimiento solar, robots autónomos o proyectos educativos.

El código permite:

Control manual de los servomotores a través de comandos Bluetooth.
Modo automático para seguir fuentes de luz.
Encendido y apagado del LED integrado.
Envío periódico de datos de los sensores a través de Bluetooth.


🛠️ Características Principales
  Control Bluetooth : Usa una aplicación móvil o terminal Bluetooth para enviar comandos al ESP32.
  Seguimiento de Luz Automático : Los servomotores se ajustan automáticamente hacia la fuente de luz más intensa.
  Interpolación Suave : Los movimientos de los servomotores son suaves gracias a un algoritmo de interpolación.
  Modo Manual y Automático : Cambia entre modos manual y automático según tus necesidades.
  Monitoreo en Tiempo Real : Envía datos de los sensores (luz y posición de los servos) cada 5 segundos.


📦 Librerías Utilizadas
  Para ejecutar este proyecto, necesitarás las siguientes librerías:

BluetoothSerial.h
  Biblioteca oficial de ESP32 para comunicación Bluetooth Serial.
  Instalación : Viene incluida en el core de Arduino para ESP32.
Servo.h
  Biblioteca estándar de Arduino para controlar servomotores.
  Instalación : Viene incluida en el IDE de Arduino.
🔧 Requisitos del Hardware

Asegúrate de tener los siguientes componentes para replicar este proyecto:
  Placa ESP32 (cualquier modelo compatible con Bluetooth).
  2 Servomotores SG90 (o equivalentes).
  LED integrado (generalmente en el pin GPIO2 del ESP32).
  Sensores de luz LDR (4 sensores: arriba, abajo, izquierda, derecha).
  Resistencias para los sensores de luz (por ejemplo, 10kΩ).
  Cables de conexión y una protoboard.
  
  🚀 Instalación y Configuración
  1. Configuración del Entorno
    Instala el IDE de Arduino .
    Agrega soporte para ESP32 siguiendo la guía oficial .
  2. Conexión del Hardware
    Conecta los componentes según el siguiente esquema:
      Servo X : Pin GPIO5.
      Servo Y : Pin GPIO18.
      Sensores de luz :
      Sensor superior: Pin GPIO34.
      Sensor inferior: Pin GPIO35.
      Sensor izquierdo: Pin GPIO32.
      Sensor derecho: Pin GPIO33.
      LED integrado : Pin GPIO2.
  3. Subir el Código
    Clona este repositorio:
    bash: git clone https://github.com/KartLinux/ProyectoESP32_Bluetooth.git
    Abre el archivo .ino en el IDE de Arduino.
    Selecciona tu placa ESP32 en el IDE (Herramientas > Placa > ESP32 Dev Module).
    Sube el código a tu placa.


⚙️ Uso del Proyecto
  <h2 style="font-size: 24px; font-weight: bold; color: #00ff00; text-align: center;">📚 Comandos Bluetooth</h2>

<div style="background-color: #282c34; color: #abb2bf; padding: 20px; border-radius: 10px; font-family: 'Courier New', monospace; line-height: 1.6;">
  <p><strong>Envía los siguientes comandos desde una aplicación Bluetooth o terminal:</strong></p>
  
  <div style="margin-left: 20px;">
    <p><span style="color: #e06c75;">AUTO</span> - Activa el modo automático (seguimiento de luz).</p>
    <p><span style="color: #e06c75;">MANUAL</span> - Activa el modo manual.</p>
    <p><span style="color: #e06c75;">LED_ON</span> - Enciende el LED.</p>
    <p><span style="color: #e06c75;">LED_OFF</span> - Apaga el LED.</p>
    <p><span style="color: #e06c75;">SERVO:X,Y</span> - Mueve los servos a las posiciones X e Y.</p>
  </div>

  <p><strong>Ejemplo de Comandos:</strong></p>
  
  <div style="background-color: #21252b; color: #d4d4d4; padding: 10px; border-radius: 5px; margin-left: 20px;">
    <p><span style="color: #569cd6;">SERVO:90,45</span> - Mueve los servos a 90° y 45°.</p>
    <p><span style="color: #569cd6;">AUTO</span> - Activa el modo automático.</p>
  </div>
</div>
    


📊 Datos Enviados por Bluetooth
El ESP32 envía datos de los sensores cada 5 segundos en el siguiente formato:
  ServoX:90,ServoY:45,Top:2000,Bottom:1500,Left:1800,Right:1600


🤝 Contribuciones
¡Las contribuciones son bienvenidas! Si encuentras errores o tienes ideas para mejorar este proyecto, sigue estos pasos:

Haz un fork del repositorio.
Crea una nueva rama (git checkout -b feature/nueva-funcionalidad).
Haz commit de tus cambios (git commit -m "Agrega nueva funcionalidad").
Sube tus cambios (git push origin feature/nueva-funcionalidad).
Abre un pull request.

📜 Licencia
Este proyecto está bajo la licencia MIT . ¡Usa y modifica el código como quieras!

🌟 Agradecimientos
A la comunidad de Arduino por proporcionar herramientas tan poderosas.
A Espressif por el soporte continuo para ESP32.
Badges
GitHub
GitHub last commit

