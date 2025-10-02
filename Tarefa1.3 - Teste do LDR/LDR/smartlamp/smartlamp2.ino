// constants won't change
#define LIGHT_SENSOR_PIN  34  // ESP32 pin GPIO36 (ADC0) connected to light sensor

void setup() {
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  int analogValue = analogRead(LIGHT_SENSOR_PIN); // read the value on analog pin
  Serial.printf("LDR Sensor Value %d.\n", analogValue);
  delay(1000);
}
