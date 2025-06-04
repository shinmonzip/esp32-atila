#include <esp_now.h>
#include <WiFi.h>

const int pino1 = 4;   // LED 1
const int pino2 = 18;  // LED 2
const int pino3 = 25;  // LED 3
const int pino4 = 14;  // LED 4
const int pino5 = 32;  // LED 5

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#define ID "equipe06"

typedef struct dados_esp {
  char id[30];
  int dado01, dado02, dado03, dado04, dado05;
} dados_esp;

dados_esp recebidoDados;

String ID_externa;
int recebido01 = 0, recebido02 = 0, recebido03 = 0, recebido04 = 0, recebido05 = 0;

bool estadoPiscar = false;
unsigned long ultimoPiscar = 0;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&recebidoDados, incomingData, sizeof(recebidoDados));
  ID_externa = recebidoDados.id;
  recebido01 = recebidoDados.dado01;
  recebido02 = recebidoDados.dado02;
  recebido03 = recebidoDados.dado03;
  recebido04 = recebidoDados.dado04;
  recebido05 = recebidoDados.dado05;

  Serial.println("--- DADOS RECEBIDOS ---");
  Serial.print("ID: "); Serial.println(ID_externa);
  Serial.print("Dado01: "); Serial.println(recebido01);
  Serial.print("Dado02: "); Serial.println(recebido02);
  Serial.print("Dado03: "); Serial.println(recebido03);
  Serial.print("Dado04: "); Serial.println(recebido04);
  Serial.print("Dado05: "); Serial.println(recebido05);
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  pinMode(pino1, OUTPUT);
  pinMode(pino2, OUTPUT);
  pinMode(pino3, OUTPUT);
  pinMode(pino4, OUTPUT);
  pinMode(pino5, OUTPUT);

  digitalWrite(pino1, LOW);
  digitalWrite(pino2, LOW);
  digitalWrite(pino3, LOW);
  digitalWrite(pino4, LOW);
  digitalWrite(pino5, LOW);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Falha ao adicionar peer");
    return;
  }
}

void desligarTodos() {
  digitalWrite(pino1, LOW);
  digitalWrite(pino2, LOW);
  digitalWrite(pino3, LOW);
  digitalWrite(pino4, LOW);
  digitalWrite(pino5, LOW);
}

void loop() {
  delay(10);

  // DADO 5: bloqueio total
  if (recebido05 == 1) {
    desligarTodos();
    return;
  }

  // DADO 3: piscar todos os LEDs
  if (recebido03 == 1) {
    unsigned long agora = millis();
    if (agora - ultimoPiscar >= 300) {
      estadoPiscar = !estadoPiscar;
      digitalWrite(pino1, estadoPiscar);
      digitalWrite(pino2, estadoPiscar);
      digitalWrite(pino3, estadoPiscar);
      digitalWrite(pino4, estadoPiscar);
      digitalWrite(pino5, estadoPiscar);
      ultimoPiscar = agora;
    }
    return; 
  }

  // DADO 1: LED 1 (ligado ou desligado)
  digitalWrite(pino1, recebido01 == 1 ? HIGH : LOW);

  // DADO 2: LED 2 (ligado ou desligado)
  digitalWrite(pino2, recebido02 == 1 ? HIGH : LOW);

  // DADO 4: LED 4 (intensidade baseada no valor)
  if (recebido04 >= 51 && recebido04 <= 100) {
    digitalWrite(pino4, HIGH);
  } else {
    digitalWrite(pino4, LOW);
  }

  // LED 3 e 5 desligados se não estiver piscando nem acionados por outro dado
  digitalWrite(pino3, LOW);
  digitalWrite(pino5, LOW);
}