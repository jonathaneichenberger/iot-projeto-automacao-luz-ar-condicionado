#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Definição dos pinos
#define PIR_PIN 34          // Pino do sensor de movimento PIR
#define DHT_PIN 12          // Pino do sensor de temperatura DHT22
#define DHT_TYPE DHT22      // Tipo do sensor DHT
#define LDR_PIN 35          // Pino do sensor de luz ambiente (LDR)

// Definição de variaveis
volatile bool movimentoDetectado = false; // Variável para verificar se o movimento foi detectado
bool ultimoEstadoSensorPIR = false; // Variável para armazenar o último estado do sensor PIR
unsigned long tempoUltimoMovimento = 0; // Variável para armazenar o último tempo de movimento detectado 
unsigned long tempoUltimoLeitura = 0; // Variável para armazenar o último tempo de leitura dos sensores 
float temperatura; // Variável para armazenar a temperatura lida pelo sensor DHT22
float umidade; // Variável para armazenar a umidade lida pelo sensor DHT22
float luminosidade; // Variável para armazenar o valor de luminosidade lido pelo LDR
float ultimaTemperatura = -999; // Variável para armazenar a última temperatura lida pelo sensor DHT22
float ultimaUmidade = -999; // Variável para armazenar a última umidade lida pelo sensor DHT22
float ultimaLuminosidade = -999; // Variável para armazenar a última luminosidade lida pelo LDR
const unsigned long tempoLimite = 30000; // Tempo limite para considerar que o movimento foi detectado
bool continuarMandandoDados = false; // Variável para controlar o envio de dados mesmo sem movimento
int contadorDadosEnviadosSemMovimento = 0; // Contador para dados enviados sem movimento
unsigned long tempoUltimoLeituraSemMovimento;

// Variaveis para a conexão com WiFi
const char* ssid = "Wokwi-GUEST"; // Nome da rede WiFi
const char* senha = ""; // Senha da rede WiFi
const int canalWiFi = 6; // Canal WiFi

// Variaveis para conexao com MQTT
const char* mqttServer = "broker.emqx.io"; // Endereço do servidor MQTT
const int mqttPort = 1883; // Porta do servidor MQTT

// Tópico MQTT
const char* topic = "senai/dataJson"; // Tópico para publicar dados em forma de JSON

DHT dht(DHT_PIN, DHT_TYPE); // Define o tipo de sensor para DHT22

WiFiClient espClient; // Cria um cliente WiFi
PubSubClient client(espClient); // Cria um cliente MQTT

void conectaWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin(ssid, senha, canalWiFi);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Conectado!");
}

void conectarBrokerMQTT() {
  client.setServer(mqttServer, mqttPort);

  int tentativas = 0;
  while (!client.connected() && tentativas < 10) { // Limite de 10 tentativas
    String clientId = "clientId-senai" + String(random(0xffff), HEX); // Gera um ID de cliente único
    Serial.print("Tentando conectar ao Broker MQTT... ");

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao Broker MQTT!");
      return;
    } else {
      Serial.print("Falha ao conectar, erro: ");
      Serial.println(client.state()); // Mostra o erro de conexão MQTT
      delay(1000);
      tentativas++;
    }
  }
  Serial.println("Não foi possível conectar ao Broker MQTT.");
}

float obterLuminosidade() {
  int leituraAnalogica = analogRead(LDR_PIN);
  float voltagem = leituraAnalogica / 4095.0 * 5;
  float resistencia = 2000 * voltagem / (1 - voltagem / 5);
  float luminosidade = pow(50 * 1e3 * pow(10, 0.7) / resistencia, (1 / 0.7));
  return luminosidade;
}

void publicarDadosBrokerMQTT(bool movimentoDetectado, float temperatura, float umidade, float luminosidade) {
  JsonDocument doc;

  doc["movimento"] = movimentoDetectado;
  doc["temperatura"] = temperatura;
  doc["umidade"] = umidade;
  doc["luminosidade"] = luminosidade;
 
  // Convertendo JSON para string
  String jsonString;
  serializeJson(doc, jsonString);

  // Publicando no MQTT
  client.publish("senai/dataJson", jsonString.c_str());

  // Debug no Serial Monitor
  Serial.print("JSON enviado: ");
  Serial.println(jsonString);
}

void IRAM_ATTR foiDetectadoMovimento() {
  movimentoDetectado = true; // Define a variável de movimento detectado como verdadeira

  tempoUltimoMovimento = millis(); // Armazena o tempo atual em milissegundos
  Serial.println("Movimento detectado!"); // Imprime mensagem de movimento detectado
}

void setup() {
  Serial.begin(115200);

  // Configura interrupção do PIR
  attachInterrupt(digitalPinToInterrupt(PIR_PIN), foiDetectadoMovimento, RISING);

  // Definindo pinos como entrada para os sensores
  pinMode(PIR_PIN, INPUT); // Configura o pino do PIR como entrada
  pinMode(LDR_PIN, INPUT); // Configura o pino do LDR como entrada
  pinMode(DHT_PIN, INPUT); // Configura o pino do DHT como entrada

  dht.begin(); // Inicializa o sensor DHT22

  conectaWiFi(); // Chama a função que conecta ao WiFi
  conectarBrokerMQTT(); // Chama a função que conecta ao broker MQTT
}

void loop() {
  unsigned long milissegundosAtuais = millis();

  // Se passaram 30s desde o último movimento, desativa o motionDetected
  if (movimentoDetectado && (milissegundosAtuais - tempoUltimoMovimento >= tempoLimite)) {
    movimentoDetectado = false;
    continuarMandandoDados = true; // Permite continuar enviando dados mesmo sem movimento
    tempoUltimoLeituraSemMovimento = milissegundosAtuais; // Atualiza o tempo da última leitura sem movimento
  }

  if(milissegundosAtuais - tempoUltimoLeitura >= 2000){
    tempoUltimoLeitura = milissegundosAtuais; // Atualiza o tempo da última leitura

    temperatura = dht.readTemperature(); // Lê a temperatura
    umidade = dht.readHumidity(); // Lê a umidade
    luminosidade = obterLuminosidade(); // Lê a luminosidade

    if (!isnan(temperatura) && !isnan(umidade) && (temperatura != ultimaTemperatura || umidade != ultimaUmidade || luminosidade != ultimaLuminosidade || movimentoDetectado != ultimoEstadoSensorPIR)) { // Verifica se a leitura falhou 
     
      ultimoEstadoSensorPIR = movimentoDetectado; // Atualiza o último estado do sensor PIR
      ultimaTemperatura = temperatura; // Atualiza a última temperatura lida
      ultimaUmidade = umidade; // Atualiza a última umidade lida
      ultimaLuminosidade = luminosidade; // Atualiza a última luminosidade lida

      Serial.println("Publicando dados no Broker MQTT...");
      publicarDadosBrokerMQTT(movimentoDetectado, temperatura, umidade, luminosidade); // Publica os dados no broker MQTT

    } else if(continuarMandandoDados && milissegundosAtuais - tempoUltimoLeituraSemMovimento >= 5000 &&   contadorDadosEnviadosSemMovimento <= 3) {
      tempoUltimoLeituraSemMovimento = milissegundosAtuais; // Atualiza o tempo da última leitura sem movimento
      publicarDadosBrokerMQTT(movimentoDetectado, temperatura, umidade, luminosidade); // Publica os dados no broker MQTT
      contadorDadosEnviadosSemMovimento++; // Incrementa o contador de dados enviados sem movimento
    } else if (continuarMandandoDados && contadorDadosEnviadosSemMovimento == 3) {
      continuarMandandoDados = false; // Desativa o envio de dados sem movimento
      contadorDadosEnviadosSemMovimento = 0; // Reseta o contador
    }
  }

  if (!client.connected()) {
    conectarBrokerMQTT();
  }
  
  client.loop(); // Mantém a conexão com o broker MQTT

}