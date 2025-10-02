// Defina os pinos de LED e LDR
// Defina uma variável com valor máximo do LDR (4000)
// Defina uma variável para guardar o valor atual do LED (10)
// int ledPin;
// int ledValue;

// int ldrPin;
// Faça testes no sensor ldr para encontrar o valor maximo e atribua a variável ldrMax
// int ldrMax;

void setup() {
  // Serial.begin(9600) inicializa uma comunicação serial
  // @ parâmetro:
  //   - 9600, define a taxa de transmissão de dados (baud rate)
  //           para 9600 bits por segundo (bps)
  //   - 115200, podemos definir um velocidade mais alta para sincronizar
  //             como Monitor Serial e não aparecer lixo em caso que velocidade
  //             de transmissão é inferior.
  Serial.begin(9600);

  // pinMode(ledPin, OUTPUT);
  // pinMode(ldrPin, INPUT);

  // Serial.println("Hello, ESP32!") envia uma menssagem de texto pela porta serial
  // O método println imprime dados na porta serial em linha.
  Serial.println("Hello, ESP32!");
  // Serial.printf("SmartLamp Initialized.\n");
}

// Função loop será executada infinitamente pelo ESP32
void loop() {
  //Obtenha os comandos enviados pela serial
  //e processe-os com a função processCommand

  // delay(1000) função de pausa de execução, ou seja, solicita
  // que o programa espere o tempo solicitado para executar a próxima instrução
  delay(1000);  // 1000 milisegundos = 1 segundo
  Serial.println("DevTitans: SmartLamp Conectado!!");
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
