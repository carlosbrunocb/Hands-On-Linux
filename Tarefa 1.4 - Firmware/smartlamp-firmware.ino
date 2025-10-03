// ===============================
// SmartLamp - Versão Completa (ESP32)
// Implementa todos os comandos do protocolo
// ===============================

// --- Definições de pinos e constantes ---
#define LDR_PIN 34          // GPIO34 no ESP32 (entrada ADC)
#define LED_PIN 32           // Pino GPIO para o LED (deve suportar PWM, ex: GPIO2 no ESP32)

// Constantes para o canal PWM do ESP32
const int freq = 5000;      // Frequência do PWM (5kHz)
const int ledChannel = 0;   // Canal PWM do ESP32 (0-15)
const int resolution = 8;   // Resolução do PWM (8 bits = 0-255)

// --- Variáveis globais de estado ---
int thresholdValue = 50;   // Limiar de ativação automática do LED (0-100)
int ledValue = 0;          // Intensidade do LED atual (0-100). Inicialmente apagado.

// --- Controle do envio periódico (GET_LDR) ---
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 2000; // 2 segundos

// --- Protótipos de Funções Auxiliares ---
int ldrGetValue();
void ledUpdate();
void processCommand(String cmd);


void setup() {
  Serial.begin(115200);
  
  // Configuração do ADC do ESP32
  analogSetAttenuation(ADC_11db); 

  // Configuração do PWM para o LED no ESP32
  // ledcSetup(ledChannel, freq, resolution);
  // ledcAttach(LED_PIN, ledChannel, resolution);
  
  // Inicializa o LED no valor inicial (0)
  ledUpdate(); 
  
  Serial.println("SmartLamp Initialized.");
}


void loop() {
  // --- Processamento de comandos recebidos via Serial ---
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // remove espaços extras
    processCommand(command);
  }

  // --- Envio periódico do valor do LDR (RES GET_LDR Z) ---
  if (millis() - lastSendTime >= sendInterval) {
    int ldrValue = ldrGetValue();
    // Usa Serial.printf para formatação eficiente e adição do '\n'
    Serial.printf("RES GET_LDR %d\n", ldrValue);
    lastSendTime = millis();
    
    // Lógica opcional de controle automático
    // Se a leitura for MAIOR que o threshold, acende o LED
    if (ldrValue > thresholdValue) {
        // Exemplo: define um brilho mínimo se acender automaticamente
        // Aqui, optamos por não mudar ledValue automaticamente para respeitar o protocolo SET_LED
    }
  }
}

// ------------------ Funções auxiliares ------------------

/**
 * @brief Normaliza o valor do LDR (0-4095) para o intervalo 0-100.
 * @return int: Valor normalizado do LDR (0-100).
 */
int ldrGetValue() {
  int raw = analogRead(LDR_PIN);    // Leitura do ADC (0 a 4095 no ESP32 com 12 bits)
  int normalized = map(raw, 0, 4095, 0, 100);
  // Garante que o valor final esteja dentro do intervalo [0, 100]
  return constrain(normalized, 0, 100);
}

/**
 * @brief Converte o ledValue (0-100) para PWM (0-255) e o aplica ao LED.
 */
void ledUpdate() {
  // ledValue (0-100) -> pwmValue (0-255)
  // int pwmValue = map(ledValue, 0, 100, 0, 255);
  // ledcWrite(ledChannel, pwmValue);
  if (ledValue > 50) {
    digitalWrite(LED_PIN, HIGH); // turn on LED
  } else {
    digitalWrite(LED_PIN, LOW); // turn on LED
  }
}

/**
 * @brief Processa o comando serial recebido e envia a resposta apropriada.
 * @param cmd: O comando completo recebido via serial.
 */
void processCommand(String cmd) {
  // Divide a string em comando e valor
  int spaceIndex = cmd.indexOf(' ');
  String command = (spaceIndex == -1) ? cmd : cmd.substring(0, spaceIndex);
  String valueStr = (spaceIndex == -1) ? "" : cmd.substring(spaceIndex + 1);

  // Comando SET_LED X
  if (command.equals("SET_LED")) {
    int value = valueStr.toInt();
    if (value >= 0 && value <= 100) {
      ledValue = value;
      ledUpdate();
      Serial.println("RES SET_LED 1");
    } else {
      Serial.println("RES SET_LED -1"); // Erro: valor inválido/fora do intervalo
    }
  } 
  
  // Comando GET_LED
  else if (command.equals("GET_LED")) {
    Serial.printf("RES GET_LED %d\n", ledValue);
  }
  
  // Comando GET_LDR
  else if (command.equals("GET_LDR")) {
    int ldrValue = ldrGetValue();
    Serial.printf("RES GET_LDR %d\n", ldrValue);
  }
  
  // Comando SET_THRESHOLD X
  else if (command.equals("SET_THRESHOLD")) {
    int value = valueStr.toInt();
    if (value >= 0 && value <= 100) {
      thresholdValue = value;
      Serial.println("RES SET_THRESHOLD 1");
    } else {
      // Usaremos a mesma resposta de erro do SET_LED para valor inválido
      Serial.println("RES SET_THRESHOLD -1"); 
    }
  } 
  
  // Comando GET_THRESHOLD
  else if (command.equals("GET_THRESHOLD")) {
    Serial.printf("RES GET_THRESHOLD %d\n", thresholdValue);
  } 
  
  // Comando Desconhecido
  else {
    Serial.println("ERR Unknown command.");
  }
}
