#include <esp_now.h>   // Biblioteca para comunicação ESP-NOW
#include <WiFi.h>      // Biblioteca para gerenciar Wi-Fi

// Endereço de broadcast (envia para todos)
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Definindo a estrutura de dados que será enviada/recebida
typedef struct dados_esp {
  char id[30];      // Identificador do dispositivo (nome)
  int dado01;       // Dado 1
  int dado02;       // Dado 2
  int dado03;       // Dado 3
  int dado04;       // Dado 4
  int dado05;       // Dado 5
} dados_esp;

// Criando variáveis para armazenar os dados a serem enviados e recebidos
dados_esp internoDados;
dados_esp recebidoDados;

// Variável para armazenar o que for digitado no Monitor Serial
String serialInput = "";

// Callback chamado após tentativa de envio de dados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Não estamos utilizando nenhuma ação específica aqui,
  // mas você poderia exibir uma mensagem de sucesso/falha
}

// Callback chamado quando dados são recebidos via ESP-NOW
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // Copia os dados recebidos para a struct local
  memcpy(&recebidoDados, incomingData, sizeof(recebidoDados));
  
  // Mostra no Monitor Serial os dados recebidos
  Serial.println("\nDados recebidos:");
  Serial.println(recebidoDados.id);
  Serial.println(recebidoDados.dado01);
  Serial.println(recebidoDados.dado02);
  Serial.println(recebidoDados.dado03);
  Serial.println(recebidoDados.dado04);
  Serial.println(recebidoDados.dado05);

  // Pisca o LED do pino 2 para indicar recebimento
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
}

// Função para enviar os dados preenchidos na struct internoDados
void enviarDados() {
  esp_now_send(broadcastAddress, (uint8_t *) &internoDados, sizeof(internoDados));
  Serial.println("Dados enviados!");
}

// Função para processar o texto digitado no Serial e preparar os dados para envio
void processSerialInput(String input) {
  input.trim(); // Remove espaços extras no começo e fim

  // Pega o índice da primeira vírgula (separador entre ID e dados)
  int primeiraVirgula = input.indexOf(',');
  if (primeiraVirgula == -1) {
    Serial.println("Formato inválido. Use: ID,dado1,dado2,dado3,dado4,dado5");
    return;
  }

  // Separa o ID e os números
  String idRecebido = input.substring(0, primeiraVirgula);
  String numeros = input.substring(primeiraVirgula + 1);

  // Vetor para armazenar os 5 números extraídos
  int valores[5];
  int i = 0;

  // Faz o parsing dos números separados por vírgula
  while (numeros.length() > 0 && i < 5) {
    int idx = numeros.indexOf(',');
    if (idx == -1) {
      valores[i] = numeros.toInt();  // Último número
      break;
    } else {
      valores[i] = numeros.substring(0, idx).toInt();
      numeros = numeros.substring(idx + 1);
      i++;
    }
  }

  // Confere se foram recebidos 5 valores
  if (i < 4) {
    Serial.println("Faltam dados! Envie 5 números separados por vírgula.");
    return;
  }

  // Preenche a struct internoDados com o que foi digitado
  idRecebido.toCharArray(internoDados.id, sizeof(internoDados.id));
  internoDados.dado01 = valores[0];
  internoDados.dado02 = valores[1];
  internoDados.dado03 = valores[2];
  internoDados.dado04 = valores[3];
  internoDados.dado05 = valores[4];

  // Envia a struct preenchida
  enviarDados();
}

void setup() {
  Serial.begin(115200); // Inicia comunicação serial
  pinMode(2, OUTPUT);   // Configura pino 2 como saída (LED)

  WiFi.mode(WIFI_STA);  // Coloca ESP em modo estação (cliente Wi-Fi)

  // Inicializa ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
    return;
  }

  // Registra callbacks de envio e recebimento
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);

  // Prepara as informações do "peer" (quem vai receber)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); // Broadcast (todos)
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Adiciona o peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Erro ao adicionar peer");
    return;
  }

  Serial.println("Pronto! Digite: ID,dado1,dado2,dado3,dado4,dado5");
}

void loop() {
  // Lê os dados do Monitor Serial
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') { // Quando pressionar Enter
      if (serialInput.length() > 0) {
        processSerialInput(serialInput); // Processa o que foi digitado
        serialInput = ""; // Limpa o buffer
      }
    } else {
      serialInput += c; // Continua adicionando caracteres digitados
    }
  }
}