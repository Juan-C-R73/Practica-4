#include <Arduino.h>
#include "BluetoothSerial.h" 

// Comprobación de seguridad: verifica que el Bluetooth Classic está habilitado en el chip
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error El Bluetooth no está habilitado. Actívalo en la configuración de la placa.
#endif

// Se crea el objeto para manejar el puerto serie Bluetooth
BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);

  // Iniciar Bluetooth Classic con el nombre que se verá al buscar desde el móvil
  SerialBT.begin("ESP32_Chat_Juan"); 
  
  Serial.println("================================================");
  Serial.println("Dispositivo Bluetooth iniciado: ESP32_Chat_Juan");
  Serial.println("Empareja tu movil y abre el terminal Bluetooth.");
  Serial.println("================================================");
}

void loop() {
  //Del PC (Monitor Serie) -> al Móvil (Bluetooth)
  if (Serial.available()) {
    char letraPC = Serial.read();
    SerialBT.write(letraPC); // Envía al móvil lo que se escribe
  }

  //Del Móvil (Bluetooth) -> al PC (Monitor Serie)
  if (SerialBT.available()) {
    char letraMovil = SerialBT.read();
    Serial.write(letraMovil); // Muestra lo que se envía desde el móvil
  }

  delay(20); // Pequeña pausa para estabilidad del bucle
}