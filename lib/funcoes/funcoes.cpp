#include "funcoes.h"

// Variaveis para conexão com a internet
const char* rede = "Wokwi-GUEST";
const char* senha = "";
const int canalWiFi = 6;

void inicializarSistema(){
  Serial.begin(115200); // Inicializa a comunicação serial a 115200 bps
  conectarWiFi(rede, senha, canalWiFi); // Conecta ao Wi-Fi
  
}

void conectarWiFi(const char* rede, const char* senha, const int canalWiFi) {
  Serial.print("Conectando ao WiFi");

  WiFi.begin(rede, senha, canalWiFi);  // Inicia a conexão com a rede Wi-Fi usando o canal especificado
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println();  // Nova linha após a conexão
  Serial.println("Conectado!");  // Mensagem de sucesso na conexão
}