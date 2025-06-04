# Projeto ESP32 - Sistemas Ciberfísicos Colaborativos

Este repositório contém códigos desenvolvidos para a disciplina de **Sistemas Ciberfísicos Colaborativos**, implementando comunicação sem fio entre dispositivos ESP32 utilizando o protocolo ESP-NOW.

## 📋 Descrição Geral

O projeto demonstra a implementação de um sistema distribuído de sensoriamento e controle usando ESP32, onde dispositivos se comunicam de forma colaborativa para formar uma rede de sensores e atuadores.

## 🗂️ Estrutura dos Códigos

### 1. `master.c++`

**Dispositivo Mestre/Controlador**

- **Função**: Atua como dispositivo central de controle da rede
- **Características**:
  - Envia comandos via comunicação serial (Monitor Serial)
  - Transmite dados via ESP-NOW em broadcast para todos os dispositivos
  - Recebe confirmações e dados de outros dispositivos
  - Interface de entrada: formato `ID,dado1,dado2,dado3,dado4,dado5`
  - LED indicador no pino 2 para sinalizar comunicação
- **Uso**: Coordenação central da rede de dispositivos

### 2. `transmissor-dht.c++`

**Sensor de Temperatura e Umidade**

- **Função**: Nó sensor que coleta dados ambientais
- **Características**:
  - Utiliza sensor DHT11 conectado no pino 19
  - Coleta temperatura e umidade a cada 5 segundos
  - Transmite dados em formato JSON via ESP-NOW
  - Inclui ID do dispositivo ("ESP1") e contador de mensagens
  - Envia dados para dispositivo intermediário específico
- **Uso**: Monitoramento ambiental distribuído

### 3. `intermediary-receptor.c++`

**Dispositivo Intermediário/Repetidor**

- **Função**: Atua como repetidor na rede de comunicação
- **Características**:
  - Recebe dados de outros dispositivos
  - Retransmite automaticamente para o dispositivo final
  - Estende o alcance da rede ESP-NOW
  - MAC Address específico configurado para o receptor final
- **Uso**: Expansão de cobertura da rede

### 4. `final-receptor.c++`

**Receptor Final/Monitor**

- **Função**: Dispositivo de monitoramento e logging
- **Características**:
  - Recebe dados JSON do dispositivo intermediário
  - Decodifica e exibe informações no monitor serial
  - Processa dados de temperatura, umidade, ID e contador
  - Interface de visualização dos dados coletados
- **Uso**: Monitoramento centralizado de dados

### 5. `leds-receptor.c++`

**Controlador de LEDs/Atuador**

- **Função**: Dispositivo atuador com controle visual
- **Características**:
  - Controla 5 LEDs conectados nos pinos 4, 18, 25, 14, 32
  - Recebe comandos via ESP-NOW e executa ações:
    - **Dado01**: Liga/desliga LED 1
    - **Dado02**: Liga/desliga LED 2
    - **Dado03**: Modo piscar todos os LEDs (300ms)
    - **Dado04**: Liga LED 4 se valor entre 51-100
    - **Dado05**: Bloqueio total (desliga todos os LEDs)
  - Feedback visual de comandos recebidos
- **Uso**: Controle remoto de dispositivos visuais

## 🔧 Tecnologias Utilizadas

- **Linguagem**: C++ (Arduino Framework)
- **Hardware**: ESP32
- **Protocolo**: ESP-NOW (comunicação sem fio ponto-a-ponto)
- **Sensores**: DHT11 (temperatura e umidade)
- **Bibliotecas**:
  - `esp_now.h` - Comunicação ESP-NOW
  - `WiFi.h` - Configuração Wi-Fi
  - `DHT.h` - Sensor DHT11
  - `ArduinoJson.h` - Manipulação de dados JSON

## 🌐 Arquitetura da Rede

```
[Master] ──────────────────> [LEDs Receptor]
                               (Broadcast)

[DHT Sensor] ──> [Intermediário] ──> [Final Receptor]
                  (Repetidor)
```

## 🚀 Como Usar

1. **Configure os endereços MAC** nos códigos conforme seus dispositivos ESP32
2. **Carregue cada código** no respectivo ESP32
3. **Para o Master**: Use o monitor serial com formato `ID,1,0,1,75,0`
4. **Para o DHT**: Conecte o sensor DHT11 no pino 19
5. **Para LEDs**: Conecte LEDs nos pinos especificados
6. **Monitore** a comunicação via serial em cada dispositivo

## 📚 Conceitos de Sistemas Ciberfísicos

Este projeto demonstra:

- **Sistemas Distribuídos**: Múltiplos dispositivos colaborando
- **Comunicação Sem Fio**: ESP-NOW para baixa latência
- **Sensoriamento**: Coleta automática de dados ambientais
- **Atuação**: Controle remoto de dispositivos físicos
- **Redundância**: Dispositivos intermediários para robustez
