#include "funcoes.h"

void setup() {
  inicializarSistema(); // Inicializa o sistema, incluindo Wi-Fi, MQTT e LCD
}

void loop() {
  if (!client.connected()) {
    conectarBrokerMQTT();
  }
  client.loop();
}