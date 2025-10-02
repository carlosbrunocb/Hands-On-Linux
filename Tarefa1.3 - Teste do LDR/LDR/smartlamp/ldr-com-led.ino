
/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-light-sensor
 */

// constants won't change
#define LIGHT_SENSOR_PIN  34  // ESP32 pin GPIO36 (ADC0) connected to light sensor
#define LED_PIN           27  // ESP32 pin GPIO22 connected to LED
#define ANALOG_THRESHOLD  500
void setup() {
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  pinMode(LED_PIN, OUTPUT); // set ESP32 pin to output mode
    Serial.begin(115200);
}

void loop() {
  int analogValue = analogRead(LIGHT_SENSOR_PIN); // read the value on analog pin
  Serial.printf("LDR Sensor Value %d.\n", analogValue);

  if (analogValue < ANALOG_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH); // turn on LED
    Serial.printf("Luminosidade baixa\n");
  }
  else {
    Serial.printf("Luminosidade alta\n");
    digitalWrite(LED_PIN, LOW);  // turn off LED
  }
}
