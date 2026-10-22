#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h> // Librería del acelerómetro integrado

//Servicio GATT para el acelerómetro (UUID aleatorio)
BLEService accelService("6A75616E-0000-0000-0000-000000000000");

//Característica con solo permisos de Lectura y Notificaciones, con máximo de 30 caracteres
BLEStringCharacteristic accelCharacteristic(
  "6A75616E-0002-0000-0000-000000000000", 
  BLERead | BLENotify, 
  30
);

unsigned long ultimaActualizacion = 0;

void setup() {
  Serial.begin(115200);

  // Inicializar el sensor IMU 
  if (!IMU.begin()) {
    Serial.println("Error al iniciar el acelerómetro IMU");
    while (1);
  }

  // Inicializar el módulo BLE
  if (!BLE.begin()) {
    Serial.println("Error al iniciar el módulo BLE");
    while (1);
  }

  // Configurar nombre del periférico y servicio en el paquete de advertising
  BLE.setLocalName("Nano33_Accel_Juan");
  BLE.setAdvertisedService(accelService);

  // Añadir la característica al servicio y el servicio al GATT
  accelService.addCharacteristic(accelCharacteristic);
  BLE.addService(accelService);

  // Valor inicial por defecto
  accelCharacteristic.writeValue("X:0.00 Y:0.00 Z:0.00");

  // Comenzar a emitir (Advertising)
  BLE.advertise();
  Serial.println("Periférico GATT listo. Esperando conexión para notificar el acelerómetro...");
}

void loop() {
  // Escuchar si el móvil se conecta
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Conectado a: ");
    Serial.println(central.address());

    // Mientras el móvil permanezca conectado
    while (central.connected()) {
      // Actualizar cada 200 ms si el acelerómetro tiene un dato nuevo disponible
      if (millis() - ultimaActualizacion >= 200) {
        if (IMU.accelerationAvailable()) {
          ultimaActualizacion = millis();

          float x, y, z;
          // Capturar los valores de los 3 ejes (en G)
          IMU.readAcceleration(x, y, z);

          // Formatear los tres valores en una cadena legible
          String datosAccel = "X:" + String(x, 2) + " Y:" + String(y, 2) + " Z:" + String(z, 2);

          // Actualizar la característica (esto dispara automáticamente la notificación BLE)
          accelCharacteristic.writeValue(datosAccel);

          // Mostrar también por el Monitor Serie
          Serial.println(datosAccel);
        }
      }
    }

    Serial.print("Desconectado de: ");
    Serial.println(central.address());
  }
}