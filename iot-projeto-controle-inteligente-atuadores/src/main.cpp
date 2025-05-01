#include "funcoes.h"

// Inicializa as variáveis de últimos dados lidos
float ultimaTemperatura = 0.0;            // Variável para armazenar a última temperatura lida
float ultimaUmidade = 0.0;                // Variável para armazenar a última umidade lida
float ultimaLuminosidade = 0.0;           // Variável para armazenar a última luminosidade lida
bool ultimoMovimento = false;             // Variável para armazenar o último movimento lido
unsigned long tempoEspera = millis();     // Variável para armazenar o tempo de espera
unsigned long tempoAtual = 0;             // Variável para armazenar o tempo atual
const unsigned long tempoLimite = 30000;  // Tempo limite para considerar que o movimento foi detectado (30 segundos)

void setup() {
  inicializarSistema();                   // Inicializa o sistema, incluindo Wi-Fi, MQTT e LCD
  exibirDadosNoLCD();                     // Exibe os dados iniciais no LCD
}

void loop() {

  if (!client.connected()) {
    conectarBrokerMQTT();
  }
  
  if (temperatura != ultimaTemperatura || umidade != ultimaUmidade || luminosidade != ultimaLuminosidade || movimento != ultimoMovimento) {
    exibirDadosNoLCD();                 // Atualiza o LCD com os dados recebidos
    ultimaTemperatura = temperatura;    // Atualiza a última temperatura lida
    ultimaUmidade = umidade;            // Atualiza a última umidade lida
    ultimaLuminosidade = luminosidade;  // Atualiza a última luminosidade lida
    ultimoMovimento = movimento;        // Atualiza o último movimento lido

    enviarDadosThingSpeak(temperatura, umidade, luminosidade); // Envia os dados para o ThingSpeak
  }

  if (movimento) {
    controleLampadas();                // Controla as lâmpadas com base no movimento
    controleArCondicionado();          // Controla o ar-condicionado com base na temperatura e movimento

  } else {
    strip.clear();                     // Desliga as lâmpadas se não houver movimento
    strip.show();                      // Atualiza o NeoPixel das lâmpadas
    stripAir.clear();                  // Desliga o Ar-condicionado se não houver movimento
    stripAir.show();                   // Atualiza o NeoPixel do Ar-condicionado
  }

  if (temperatura >= 20 && temperatura <= 25 && (tempoEspera - tempoAtual) <= tempoLimite) { // Verifica se a temperatura está dentro do intervalo desejado
    abrirJanelas();                                   // Chama a função para abrir as janelas
  } else if (temperatura < 20 || temperatura > 25) {  // Verifica se o tempo limite foi atingido
    fecharJanelas();                                  // Chama a função para fechar as janelas
  } else if (!movimento){
    fecharJanelas();                                  // Chama a função para fechar as janelas
  } 
    
  

  // Mantém a conexão com o broker MQTT
  client.loop();
}