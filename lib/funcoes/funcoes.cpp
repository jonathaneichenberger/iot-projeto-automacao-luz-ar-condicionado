#include "funcoes.h"

// Inicializando as variáveis globais
bool movimento = false;
float temperatura = 0.0;
float umidade = 0.0;
float luminosidade = 0.0;
unsigned int counter = 1;

WiFiClient espClient;               // Instância de Cliente Wi-Fi para comunicação
PubSubClient client(espClient);     // Definindo o cliente MQTT

// Definição do LCD (RS, E, D4, D5, D6 e D7)
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7); 

// Configuração dos NeoPixels
Adafruit_NeoPixel strip(PIXEL_COUNT, LAMP_PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripAir(PIXEL_COUNT, AIR_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Definição do número de servos
Servo servo[NUM_SERVOS];           // Array de servos declared as external

void inicializarSistema(){
  Serial.begin(115200);            // Inicializa a comunicação serial a 115200 bps

  // Definição dos pinos do LCD
  pinMode(LAMP_PIXEL_PIN, OUTPUT); // Inicializa o pino do NeoPixel das lâmpadas como saída
  pinMode(AIR_PIXEL_PIN, OUTPUT);  // Inicializa o pino do NeoPixel do ar-condicionado como saída              
  
  // Inicializa o LCD
  lcd.begin(20, 4);                // Inicializa o LCD com 20 colunas e 4 linhas

  // Inicializa NeoPixels
  strip.begin();                   // Inicializa o NeoPixel das lâmpadas
  strip.show();                    // Inicializa com os LED off  
  stripAir.begin();                // Inicializa o NeoPixel do ar-condicionado
  stripAir.show();                 // Inicializa com os LED off  

  conectarWiFi();                  // Conecta ao Wi-Fi
  client.setCallback(callback);    // Define a função de callback para mensagens recebidas
  conectarBrokerMQTT();            // Conecta ao broker MQTT

  conectarThingSpeak();            // Conecta ao ThingSpeak
  // Exibe mensagem inicial no LCD
  lcd.setCursor(2, 0);
  lcd.print("  Monitoramento  ");
  delay(2000);
  lcd.clear();

  for( int i = 0; i < NUM_SERVOS; i++) { 
    servo[i].attach(i + 23 + i);    // Define os pinos dos servos (23, 25, 27)
    servo[i].write( 0 );            // Define a posição inicial do servo
  }
}

void conectarWiFi() {
  Serial.print("Conectando ao WiFi...");

  WiFi.begin(rede, senha, canalWiFi); // Inicia a conexão com a rede Wi-Fi usando o canal especificado

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("Conectado!");       // Mensagem de sucesso na conexão
}

void conectarBrokerMQTT() {
  client.setServer(servidorMQTT, portaMQTT);                           // Define o servidor MQTT e a porta para conexão
  while (!client.connected()) {                                        // Loop até conectar ao broker MQTT
    String clientId = "ESP32-Receiver-senai" + String(random(0xffff), HEX); // Gera um ID único para o cliente MQTT
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

void conectarThingSpeak() {
  ThingSpeak.begin(espClient);
}

void enviarDadosThingSpeak(float temperature, float humidity, float luminosity) {
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, luminosity);

  // Enviar os dados
  int httpCode = ThingSpeak.writeFields(channelID, writeAPIKey);
  if (httpCode == 200) {
    Serial.println("Dados enviados com sucesso ao ThinkSpeak nº " + String(counter));
    Serial.println("-------------------------------");
    counter++;
  } else {
    Serial.println("Falha ao enviar dados");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);                                       // Exibe o nome do tópico MQTT que recebeu a mensagem
  char mensagem[length + 1];                                   // Cria um buffer para armazenar a mensagem recebida
  memcpy(mensagem, payload, length);                           // Copia os dados da mensagem para o buffer
  mensagem[length] = '\0';                                     // Adiciona o caractere nulo ao final para formar uma string válida
  JsonDocument doc;                                            // Cria um objeto JSON para armazenar os dados recebidos vazio (tipo dinâmico)                   
  DeserializationError error = deserializeJson(doc, mensagem); // Converte a string JSON recebida para um objeto JSON
  if (error) {                                                 // Verifica se houve erro na conversão do JSON
    Serial.print("Erro ao analisar JSON: ");                   // Mensagem de erro se a conversão falhar                
    Serial.println(error.c_str());                             // Exibe a mensagem de erro
    return;                                                    // Retorna da função se houve erro na conversão                       
  }

  // Extrai os valores do JSON recebido
  movimento = doc["movimento"];                          // Extrai o valor do movimento (booleano) 
  temperatura = doc["temperatura"];                      // Extrai o valor da temperatura (float)                 
  umidade = doc["umidade"];                              // Extrai o valor da umidade (float)        
  luminosidade = doc["luminosidade"];                    // Extrai o valor da luminosidade (float)             

  // Exibe os dados recebidos no monitor serial
  Serial.println("--- Dados Recebidos ---");
  Serial.print("Movimento: "); Serial.println(movimento ? "Sim" : "Não");
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");
  Serial.print("Umidade: "); Serial.print(umidade); Serial.println(" %");
  Serial.print("Luminosidade: "); Serial.print(luminosidade); Serial.println(" lux");
  Serial.println("------------------------");
  
}

void exibirDadosNoLCD() {
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatura);
  lcd.print(" C    ");  

  lcd.setCursor(0, 1);
  lcd.print("Mov: ");
  lcd.print(movimento ? "DETECTADO" : "NENHUM    ");

  lcd.setCursor(0, 2);
  lcd.print("Lampadas: ");
  lcd.print(movimento && luminosidade <= 700 ? "LIGADAS   " : "DESLIGADAS");
  
  lcd.setCursor(0, 3);
  lcd.print("Ar Cond: ");
  lcd.print((temperatura < 20 || temperatura > 25) && movimento ? "LIGADO   " : "DESLIGADO");
}

void controleArCondicionado(){
  uint32_t cor;
  if (temperatura > 25) {            // RGB: Red, Green, Blue valor máximo 255.
    cor = stripAir.Color(0, 0, 255); // Azul: ar-condicionado ligado no frio
  } else if (temperatura < 20) {
    cor = stripAir.Color(255, 0, 0); // Vermelho: ar-condicionado ligado no quente
  } else {
    stripAir.clear();
    stripAir.show();
    return;
  }

  // Acende todos os LEDs do NeoPixel do ar-condicionado
  for (int i = 0; i < PIXEL_COUNT; i++) {
    stripAir.setPixelColor(i, cor);
  }
  stripAir.show();
}

void controleLampadas() {
  int brilho;

  if (movimento) {
    if (luminosidade <= 300) {  
      brilho = map(luminosidade, 0, 300, 255, 150);
      uint32_t corBranca = strip.ColorHSV(0, 0, brilho);
      for (int i = 0; i < PIXEL_COUNT; i++) {
        strip.setPixelColor(i, corBranca);
      }
    } else if (luminosidade > 300 && luminosidade <= 700) {
      brilho = map(luminosidade, 300, 700, 150, 0);
      uint32_t corAmarela = strip.ColorHSV(40 * 65536 / 360, 100, brilho);
      for (int i = 0; i < PIXEL_COUNT; i++) {
        strip.setPixelColor(i, corAmarela);
      }
    } else {
      strip.clear();
    }
  
    strip.show();
  } else {
    strip.clear();
    strip.show();
  }
}

void abrirJanelas() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    servo[i].write(180); // Abre a janela
  }
}

void fecharJanelas() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    servo[i].write(0); // Fecha a janela
  }
}