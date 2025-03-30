#ifndef FUNCOES_H
#define FUNCOES_H

#include <Arduino.h>          // Biblioteca principal para programação em placas Arduino
#include <WiFi.h>             // Permite conexão Wi-Fi para ESP32 e ESP8266
#include <PubSubClient.h>     // Implementa o protocolo MQTT para comunicação IoT
#include <ArduinoJson.h>      // Facilita a manipulação de dados em formato JSON
#include <LiquidCrystal.h>    // Controla displays LCD baseados no controlador HD44780
#include <Adafruit_NeoPixel.h>// Controla LEDs RGB endereçáveis da Adafruit (NeoPixels)
#include <ThingSpeak.h>       // Facilita a comunicação com a plataforma IoT ThingSpeak

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

// Protótipos das funções
void inicializarSistema();
void conectarWiFi();
void conectarMQTT();
 

// Constantes para conexão com a internet
constexpr const char* rede = "Wokwi-GUEST";            // Nome da rede Wi-Fi
constexpr const char* senha = "";                      // Senha da rede Wi-Fi
constexpr const int canalWiFi = 6;                     // Canal Wi-Fi
// Constantes para conexão do Broker com protocolo MQTT
constexpr const char* servidorMQTT = "broker.emqx.io"; // Endereço do servidor MQTT
constexpr const int portaMQTT = 1883;                  // Porta do servidor MQTT
constexpr const char* topico = "senai/dataJson";       // Tópico MQTT para publicação/assinatura
extern WiFiClient espClient;                           // Cliente Wi-Fi para comunicação
extern PubSubClient client;                            // Cliente MQTT para comunicação

#endif