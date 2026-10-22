#include <Arduino.h>
#include <ArduinoBLE.h>

void setup() {
  Serial.begin(115200);
  

  if (!BLE.begin()) {
    Serial.println("Error al iniciar el módulo BLE");
    while (1);
  }

  // Nombre visible en el escaneo desde el movil
  BLE.setLocalName("Nano33_JuanCR");

  // vector iBeacon de 25 bytes
  uint8_t iBeaconData[25] = {
    //0x4C, 0x00, // Company ID de Apple 
    0xFF, 0xFF, //cambiado para que pueda verse en la aplicacion
    0x02,       // Para identificar que es un iBeacon
    0x15,       // Longitud restante
    
    // UUID de 16 bytes, donde se inserta el nombre y se rellena con ceros hasta alcanzar
    //16 bytes, con comillas se traduce a ASCII
    'J', 'u', 'a', 'n', 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    0x00, 0x01, // Major, numero de grupo, por si se agrupan varias balizas
    0x00, 0x01, // Minor, numero individual de cada baliza del major
    0xC5        // Potencia TX a 1 metro en complemento a 2 (0xC5 = -59 dBm)
  };

  // Para insertar el array de 25 bytes dentro del campo datos de fabricante del paquete 
  //Bluetooth
  BLE.setManufacturerData(iBeaconData, sizeof(iBeaconData));

  // Para que el módulo BLE empiece a emitir el mensaje de advertising al aire 
  //de forma continua
  BLE.advertise();
  Serial.println("Emitiendo iBeacon con UUID: 4A75616E-0000-0000-0000-000000000000 (Juan)");
}

void loop() {
  // Mantiene los eventos BLE activos
  BLE.poll();
}