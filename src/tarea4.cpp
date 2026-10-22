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

//Característica con solo permiso de escritura
BLEByteCharacteristic accelControlChar(
  "6A75616E-0003-0000-0000-000000000000", 
  BLEWrite
);
bool acelerometroActivo = true;
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
  accelService.addCharacteristic(accelControlChar);
  BLE.addService(accelService);

  // Valor inicial por defecto
  accelCharacteristic.writeValue("X:0.00 Y:0.00 Z:0.00");
  digitalWrite(LED_BUILTIN, HIGH); // LED encendido = acelerómetro activo por defecto
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
        
      //Comprobar si el móvil ha escrito 
      if (accelControlChar.written()) {
        uint8_t comando = accelControlChar.value();
        
        if (comando == 1) {
          acelerometroActivo = true;
          digitalWrite(LED_BUILTIN, HIGH); // Enciende el LED al activar
          Serial.println("-> Comando recibido (1): Acelerómetro ACTIVADO");
        } else {
          acelerometroActivo = false;
          digitalWrite(LED_BUILTIN, LOW);  // Apaga el LED al desactivar
          accelCharacteristic.writeValue("DESACTIVADO");
          Serial.println("-> Comando recibido (0): Acelerómetro DESACTIVADO");
        }
     }
      
      // Actualizar cada 200 ms si el acelerómetro tiene un dato nuevo disponible
      if (acelerometroActivo && (millis() - ultimaActualizacion >= 200)) {
        if (IMU.accelerationAvailable()) {
          ultimaActualizacion = millis();

          float x, y, z;
          // Capturar los valores de los 3 ejes (en G)
          IMU.readAcceleration(x, y, z);

          // Formatear los tres valores en una cadena legible
          String datosAccel = "X:" + String(x, 2) + " Y:" + String(y, 2) + " Z:" + String(z, 2);

          // Actualizar la característica 
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