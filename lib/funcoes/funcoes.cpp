#include "funcoes.h"

WiFiClient espClient;
PubSubClient client(espClient);

void inicializarSistema(){
  Serial.begin(115200); // Inicializa a comunicação serial a 115200 bps
  conectarWiFi();       // Conecta ao Wi-Fi
  conectarMQTT();       // Conecta ao broker MQTT
}

void conectarWiFi() {
  Serial.print("Conectando ao WiFi...");

  WiFi.begin(rede, senha, canalWiFi);  // Inicia a conexão com a rede Wi-Fi usando o canal especificado

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("Conectado!");  // Mensagem de sucesso na conexão
}

void conectarMQTT() {
  client.setServer(servidorMQTT, portaMQTT);

  while (!client.connected()) {
    String clientId = "ESP32-Receiver-" + String(random(0xffff), HEX); // Gera um ID único para o cliente MQTT
    Serial.print("Conectando ao Broker MQTT...");
    if (client.connect(clientId.c_str())) { // Conecta ao broker MQTT
      Serial.println("Conectado!");
      client.subscribe(topico); // Inscreve-se no tópico especificado
    } else {
      Serial.print(" Falha ao conectar. Erro: ");
      Serial.println(client.state()); // Mensagem de erro se a conexão falhar
      delay(1000);
    }
  }
}