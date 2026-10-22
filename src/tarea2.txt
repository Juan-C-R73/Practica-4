#include <Arduino.h>
#include <ArduinoBLE.h>


//UUID de 128 bits al azar
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");

// Variable de 1 Byte donde se guarda el color actual
//con permisos de Lectura (BLERead) y Escritura (BLEWrite) para el móvil
BLEByteCharacteristic colorCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

// Función para cambiar el color del LED RGB, HIGH=apagado
void cambiarColorLED(uint8_t codigoColor) {
  switch (codigoColor) {
    case 1: // ROJO
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
      Serial.println("Color cambiado a: ROJO");
      break;
    case 2: // VERDE
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, HIGH);
      Serial.println("Color cambiado a: VERDE");
      break;
    case 3: // AZUL
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, LOW);
      Serial.println("Color cambiado a: AZUL");
      break;
    case 4: // AMARILLO (Rojo + Verde)
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, HIGH);
      Serial.println("Color cambiado a: AMARILLO");
      break;
    case 5: // CIAN (Verde + Azul)
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, LOW);
      Serial.println("Color cambiado a: CIAN");
      break;
    case 6: // MAGENTA (Rojo + Azul)
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, LOW);
      Serial.println("Color cambiado a: MAGENTA");
      break;
    case 7: // BLANCO (Todos encendidos)
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, LOW);
      digitalWrite(LEDB, LOW);
      Serial.println("Color cambiado a: BLANCO");
      break;
    default: // 0 o cualquier otro número -> APAGADO
      digitalWrite(LEDR, HIGH);
      digitalWrite(LEDG, HIGH);
      digitalWrite(LEDB, HIGH);
      Serial.println("LED APAGADO");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Configurar los 3 pines del LED RGB como salida
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // Empezar con el LED RGB apagado 
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  // Iniciar el módulo BLE
  if (!BLE.begin()) {
    Serial.println("Error al iniciar BLE");
    while (1);
  }

  // Configurar el nombre del periférico y el servicio anunciado
  BLE.setLocalName("Nano33_LED_Juan");
  BLE.setAdvertisedService(ledService);

  // Incluye la característica (la variable de un byte) dentro del servicio, y el servicio al GATT del BLE
  ledService.addCharacteristic(colorCharacteristic);
  BLE.addService(ledService);

  // Valor inicial de la característica 
  colorCharacteristic.writeValue(0);

  // Empezar a emitir (Advertising)
  BLE.advertise();
  Serial.println("Periférico GATT listo. Esperando conexión para cambiar el color del LED...");
}

void loop() {
  // Escuchar si algún dispositivo central se conecta
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Conectado al móvil: ");
    Serial.println(central.address());

    // Mientras el móvil siga conectado:
    while (central.connected()) {
      // Comprobar si desde el móvil se ha escrito un nuevo valor en la característica
      if (colorCharacteristic.written()) {
        uint8_t valorRecibido = colorCharacteristic.value();
        cambiarColorLED(valorRecibido);
      }
    }

    Serial.print("Desconectado del móvil: ");
    Serial.println(central.address());
  }
}