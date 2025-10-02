#define LED_BLINKS_CYCLES 10 // Ciclo de vezes que o LED vai piscar
#define LED_DELAY_TIME 2000  // Tempo de duração do delay (2s)
#define LDR_BOOT_DELAY_TIME 1000 // delay de 1s
#define LDR_DELAY_TIME 2000      // delay de 2s

// Defina os pinos de LED e LDR
// Defina uma variável com valor máximo do LDR (4000)
// Defina uma variável para guardar o valor atual do LED (10)
const int ledPin = 32; // LED PINOUT
// int ledValue;

int ldrPin = 34; // LDR PINOUT
// Faça testes no sensor ldr para encontrar o valor maximo e atribua a variável ldrMax
// int ldrMax;

int blinkCycloCount = 0;

void setup() {
  // Serial.begin(9600) inicializa uma comunicação serial
  // @ parâmetro:
  //   - 9600, define a taxa de transmissão de dados (baud rate)
  //           para 9600 bits por segundo (bps)
  //   - 115200, podemos definir um velocidade mais alta para sincronizar
  //             como Monitor Serial e não aparecer lixo em caso que velocidade
  //             de transmissão é inferior.
  Serial.begin(9600);

  // pinMode: função que define o modo de operação do pino digital
  // Define o pino do LED como saída digital.
  pinMode(ledPin, OUTPUT);
  // pinMode(ldrPin, INPUT);

  // Serial.println("Hello, ESP32!") envia uma menssagem de texto pela porta serial
  // O método println imprime dados na porta serial em linha.
  Serial.printf("SmartLamp Initialized.\n");

  delay(LDR_BOOT_DELAY_TIME);
  Serial.println("Leitura do LDR iniciada...");
  
}

// Função loop será executada infinitamente pelo ESP32
void loop() {
  //Obtenha os comandos enviados pela serial
  //e processe-os com a função processCommand

  // analogRead: função de leitura do sinal de tensão contínuo em um pino de entrada
  // Leitura de tensão gerada pelo sensor de lux do LDR 
  int ldrValue = analogRead(ldrPin);  
  
  Serial.print("Valor Lido pelo LDR:");
  Serial.println(ldrValue);
  delay(LDR_DELAY_TIME);

  // Liga e desliga o LED por um quantidade de ciclos definido em LED_BLINKS_CYCLES
  if(blinkCycloCount++ < LED_BLINKS_CYCLES) {

    // digitalWrite: função que define o nível de tensão de um pino de saída digital
    // delay: função de pausa de execução, ou seja, solicita
    // que o programa espere o tempo solicitado para executar a próxima instrução
    digitalWrite(ledPin, HIGH); // Aplica nível lógico baixo (3.3V ou 5V), ligando o LED.
    delay(LED_DELAY_TIME);
    digitalWrite(ledPin, LOW);  // Aplica nível lógico baixo (0V), desligando o LED.
    delay(LED_DELAY_TIME);

    Serial.printf("%dº ciclo\n", blinkCycloCount);
  }
}


void processCommand(String command) {
  // compare o comando com os comandos possíveis e execute a ação correspondente
}

// Função para atualizar o valor do LED
void ledUpdate() {
  // Valor deve convertar o valor recebido pelo comando SET_LED para 0 e 255
  // Normalize o valor do LED antes de enviar para a porta correspondente
}

// Função para ler o valor do LDR
int ldrGetValue() {
  // Leia o sensor LDR e retorne o valor normalizado entre 0 e 100
  // faça testes para encontrar o valor maximo do ldr (exemplo: aponte a lanterna do celular para o sensor)
  // Atribua o valor para a variável ldrMax e utilize esse valor para a normalização
}
