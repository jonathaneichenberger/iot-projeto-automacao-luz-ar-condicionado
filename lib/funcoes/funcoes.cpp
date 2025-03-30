#include "funcoes.h"

WiFiClient espClient;
PubSubClient client(espClient);

void inicializarSistema(){
  Serial.begin(115200);         // Inicializa a comunicação serial a 115200 bps
  conectarWiFi();               // Conecta ao Wi-Fi
  client.setCallback(callback); // Define a função de callback para mensagens recebidas
  conectarBrokerMQTT();         // Conecta ao broker MQTT
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

void conectarBrokerMQTT() {
  client.setServer(servidorMQTT, portaMQTT);                           // Define o servidor MQTT e a porta para conexão
  while (!client.connected()) {                                        // Loop até conectar ao broker MQTT
    String clientId = "ESP32-Receiver-" + String(random(0xffff), HEX); // Gera um ID único para o cliente MQTT
    Serial.print("Conectando ao Broker MQTT...");                      // Mensagem de tentativa de conexão              
    if (client.connect(clientId.c_str())) {                            // Tenta conectar ao broker MQTT com o ID gerado
      Serial.println("Conectado!");                                    // Mensagem de sucesso na conexão             
      client.subscribe(topico);                                        // Inscreve-se no tópico MQTT especificado
    } else {                                                           // Se a conexão falhar                                      
      Serial.print(" Falha ao conectar. Erro: ");                      // Mensagem de erro se a conexão falhar
      Serial.println(client.state());                                  // Exibe o estado do erro se a conexão falhar
      delay(1000);                                                     // Aguarda 1 segundo antes de tentar novamente
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);                                       // Exibe o nome do tópico MQTT que recebeu a mensagem
  char mensagem[length + 1];                                   // Cria um buffer para armazenar a mensagem recebida
  memcpy(mensagem, payload, length);                           // Copia os dados da mensagem para o buffer
  mensagem[length] = '\0';                                     // Adiciona o caractere nulo ao final para formar uma string válida
  JsonDocument doc;                                            // Cria um objeto JSON para armazenar os dados recebidos vazio (tipo dinâmico)                   
  //doc["answer"] = 80;                                          // Adiciona um par chave-valor ao JSON, onde "answer" tem o valor 80
  DeserializationError error = deserializeJson(doc, mensagem); // Converte a string JSON recebida para um objeto JSON
  if (error) {                                                 // Verifica se houve erro na conversão do JSON
    Serial.print("Erro ao analisar JSON: ");                   // Mensagem de erro se a conversão falhar                
    Serial.println(error.c_str());                             // Exibe a mensagem de erro
    return;                                                    // Retorna da função se houve erro na conversão                       
  }

  // Extrai os valores do JSON recebido
  bool movimento = doc["movimento"];                           // Extrai o valor do movimento (booleano) 
  float temperatura = doc["temperatura"];                      // Extrai o valor da temperatura (float)                 
  float umidade = doc["umidade"];                              // Extrai o valor da umidade (float)        
  float luminosidade = doc["luminosidade"];                    // Extrai o valor da luminosidade (float)             

  // Exibe os dados recebidos no monitor serial
  Serial.println("--- Dados Recebidos ---");
  Serial.print("Movimento: "); Serial.println(movimento ? "Sim" : "Não");
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");
  Serial.print("Umidade: "); Serial.print(umidade); Serial.println(" %");
  Serial.print("Luminosidade: "); Serial.print(luminosidade); Serial.println(" lux");
  Serial.println("------------------------");
}
