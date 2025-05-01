#ifndef FUNCOES_H
#define FUNCOES_H

#include <Arduino.h>          // Biblioteca principal para programação em placas Arduino
#include <WiFi.h>             // Permite conexão Wi-Fi para ESP32 e ESP8266
#include <PubSubClient.h>     // Implementa o protocolo MQTT para comunicação IoT
#include <ArduinoJson.h>      // Facilita a manipulação de dados em formato JSON
#include <LiquidCrystal.h>    // Controla displays LCD baseados no controlador HD44780
#include <Adafruit_NeoPixel.h>// Controla LEDs RGB endereçáveis da Adafruit (NeoPixels)
#include <ThingSpeak.h>       // Facilita a comunicação com a plataforma IoT ThingSpeak
#include <ESP32Servo.h>       // Permite controle de servos com ESP32

// Definição dos pinos
#define LAMP_PIXEL_PIN 6  // Pino do NeoPixel das lâmpadas
#define AIR_PIXEL_PIN 32  // Pino do NeoPixel do ar-condicionado
#define PIXEL_COUNT 16    // Quantidade de LEDs nos NeoPixels
#define LCD_RS 19         // Pino RS do LCD
#define LCD_E 18          // Pino E do LCD
#define LCD_D4 5          // Pino D4 do LCD
#define LCD_D5 21         // Pino D5 do LCD
#define LCD_D6 2          // Pino D6 do LCD
#define LCD_D7 15         // Pino D7 do LCD
#define NUM_SERVOS 3      // Número de servos

// Protótipos das funções
void inicializarSistema();
void conectarWiFi();
void conectarBrokerMQTT();
void conectarThingSpeak();
void enviarDadosThingSpeak(float temperatura, float umidade, float luminosidade);
void callback(char* topic, byte* payload, unsigned int length);
void exibirDadosNoLCD();
void controleArCondicionado();
void controleLampadas();
void abrirJanelas();
void fecharJanelas();

 
// Constantes para conexão com a internet
constexpr const char* rede = "Wokwi-GUEST";            // Nome da rede Wi-Fi
constexpr const char* senha = "";                      // Senha da rede Wi-Fi
constexpr const int canalWiFi = 6;                     // Canal Wi-Fi

// Constantes para conexão do Broker com MQTT
constexpr const char* servidorMQTT = "broker.emqx.io"; // Endereço do servidor MQTT
constexpr const int portaMQTT = 1883;                  // Porta do servidor MQTT
constexpr const char* topico = "senai/dataJson";       // Tópico MQTT para publicação/assinatura

constexpr const unsigned long channelID = 2884497;
constexpr const char* writeAPIKey = "H39UCHX655KFTXM6";

// Variaveis globais para receber dados do Broker MQTT
extern bool movimento;                  // Variável para armazenar o valor de movimento
extern float temperatura;               // Variável para armazenar a temperatura
extern float umidade;                   // Variável para armazenar a umidade
extern float luminosidade;              // Variável para armazenar a luminosidade
extern unsigned long tempo;             // Variável para armazenar o tempo de espera
extern unsigned long tempoAtual;        // Variável para armazenar o tempo atual
extern const unsigned long tempoLimite; // Tempo limite para considerar que o movimento foi detectado


extern WiFiClient espClient;        // Instância de Cliente Wi-Fi para comunicação
extern PubSubClient client;         // Instância de Cliente MQTT para comunicação
extern LiquidCrystal lcd;           // Instância do LCD
extern Adafruit_NeoPixel strip;     // Instância do NeoPixel das lâmpadas
extern Adafruit_NeoPixel stripAir;  // Instância do NeoPixel do ar-condicionado
extern Servo servo[NUM_SERVOS];     // Array de servos



#endif