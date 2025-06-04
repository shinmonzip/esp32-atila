#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ==== CONFIGURAÇÕES ====
const char* ssid = "SEU_WIFI";           // <--- Coloque o nome do seu Wi-Fi
const char* password = "SENHA_WIFI";     // <--- Coloque a senha do seu Wi-Fi
const char* serverUrl = "https://SEU-FLOWFUSE.flowfuse.com/dadosesp"; // <--- URL do seu endpoint Node-RED/FlowFuse

// ==== PINOS ====
const int pino1 = 4;
const int pino2 = 18;
const int pino3 = 25;
const int pino4 = 14;
const int pino5 = 32;

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

// ==== ENVIA DADOS PARA FLOWFUSE ====
void enviarParaNuvem() {
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Cria o JSON a ser enviado
    StaticJsonDocument<200> json;
    json["id"] = ID_externa;
    json["dado01"] = recebido01;
    json["dado02"] = recebido02;
    json["dado03"] = recebido03;
    json["dado04"] = recebido04;
    json["dado05"] = recebido05;

    String jsonString;
    serializeJson(json, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0) {
      Serial.print("Enviado para FlowFuse, código: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro no envio: ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado.");
  }
}

// ==== RECEBE DADOS ESP-NOW ====
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

  enviarParaNuvem(); // Envia para a nuvem ao receber
}

void desligarTodos() {
  digitalWrite(pino1, LOW);
  digitalWrite(pino2, LOW);
  digitalWrite(pino3, LOW);
  digitalWrite(pino4, LOW);
  digitalWrite(pino5, LOW);
}

void setup() {
  Serial.begin(115200);

  pinMode(pino1, OUTPUT);
  pinMode(pino2, OUTPUT);
  pinMode(pino3, OUTPUT);
  pinMode(pino4, OUTPUT);
  pinMode(pino5, OUTPUT);
  desligarTodos();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado. IP: " + WiFi.localIP().toString());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW");
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

void loop() {
  delay(10);

  if (recebido05 == 1) {
    desligarTodos();
    return;
  }

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

  digitalWrite(pino1, recebido01 == 1 ? HIGH : LOW);
  digitalWrite(pino2, recebido02 == 1 ? HIGH : LOW);
  digitalWrite(pino4, (recebido04 >= 51 && recebido04 <= 100) ? HIGH : LOW);

  digitalWrite(pino3, LOW);
  digitalWrite(pino5, LOW);
}