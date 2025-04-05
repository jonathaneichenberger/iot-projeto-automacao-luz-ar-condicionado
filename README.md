# 📡 Automatização e Controle Inteligente de Iluminação e Climatização do Ambiente

> Projeto desenvolvido nas aulas de **Internet das Coisas (IoT)** no **SENAI**, com o objetivo de automatizar o controle de lâmpadas e ar-condicionado, otimizando o consumo de energia. Através da conectividade e coleta de dados, o sistema monitora a iluminação e a temperatura do ambiente, permitindo uma gestão mais eficiente e sustentável dos recursos.

---

## 📌 Índice
- [Sobre o Projeto](#sobre-o-projeto)
- [Comunicação MQTT](#comunicacao-mqtt)
- [Módulo 1:Sensores](#modulo-1)
- [Módulo 2:Atuadores](#modulo-2)
- [Resumo do Fluxo](#resumo-fluxo)

---

<a id="sobre-o-projeto"></a>
## 🧐 Sobre o Projeto

### 🎯 Objetivo do Projeto
O projeto tem como objetivo automatizar o controle de lâmpadas e do ar-condicionado por meio da **Internet das Coisas (IoT)**, promovendo uma gestão eficiente do consumo de energia. Através da conectividade e da coleta de dados, o sistema monitora a iluminação e a temperatura do ambiente, ajustando automaticamente os dispositivos para otimizar o uso dos recursos.

### ⚡ Problema que o Projeto Resolve
O desperdício de energia em ambientes onde lâmpadas e ar-condicionado permanecem ligados desnecessariamente é um problema comum. Esse projeto busca solucionar essa questão ao implementar um **sistema inteligente** que aciona e desliga os dispositivos conforme a presença de pessoas e as condições do ambiente. Dessa forma, reduz-se o consumo excessivo de eletricidade, promovendo **economia e sustentabilidade**.

---

<a id="comunicacao-mqtt"></a>
## 📡 Comunicação MQTT

- **Broker:** `broker.emqx.io`
- **Tópico utilizado:** `senai/dataJson`
- **Formato das mensagens:** JSON

---

<a id="modulo-1"></a>
## 🔍 Módulo 1: Sensores

Este módulo é responsável por coletar os dados dos sensores conectados ao ESP32 e enviar as informações via MQTT.

### Sensores utilizados:
- Sensor de Temperatura
- Sensor de Luminosidade
- Sensor de Presença (PIR)

### Funcionamento:
1. O ESP32 conecta-se a rede WiFi e ao Broker via MQTT. 
2. O ESP32 lê os dados de todos os sensores.
* Leituras:
   * PIR (pino 34): Detecta presença humana. Usa interrupção via attachInterrupt() com IRAM_ATTR para garantir resposta rápida.
   * DHT22 (pino 12): Mede temperatura e umidade.
   * LDR (pino 35): Mede luminosidade por leitura analógica e cálculo de resistência/lux.
* Lógica de funcionamento:
   * Se detectar movimento:
      * Envia os dados dos sensores.
   * Se não detectar:
      * Espera 10 segundos e ainda assim, envia até 3 mensagens extras com os dados para manter a atualização.
      * Evita envio repetitivo se os dados não mudarem.
        
3. O algoritmo então, irá mandar os dados iniciais e depois analisa se deve ou não enviar dados para o broker, verificando se houve alguma mudança em algum dado previamente coletado.
 
4. Os dados são formatados em JSON, como o exemplo abaixo.
   
   **Exemplo de JSON criado:**
   ```json
   {
     "movimento": true,
     "temperatura": 14,
     "umidade": 40,
     "luminosidade": 499.6338
   }
   ```
5. Depois do JSON criado, ele é Serializado para poder enviar os dados em uma única String, como exemplo abaixo.
   
   **Exemplo de payload JSON enviado:**
   ```json
   {"movimento":true,"temperatura":14,"umidade":40,"luminosidade":499.6338}
   ```
6. O ESP32 publica os dados do JSON serializado no tópico `senai/dataJson` no broker MQTT.

---

<a id="modulo-2"></a>
## 💡 Módulo 2: Atuadores

Este módulo recebe os dados via MQTT desserealizando para o formato JSON, adicionando os dados as varoáveis de controle. Com os dados em mão, a lógica implementada aciona os NeoPixels e os servos, que representam as lâmpadas, ar-condicionado e janelas, com base nas informações dos sensores.

### Atuadores utilizados:
- Tiras de LED **NeoPixel** (representando ar-condicionado e lâmpadas)
- **Servos** (representando os motores para abertura das janelas em temperaturas "agrádaveis")

### Funcionamento:
1. O ESP32 conecta-se a rede WiFi e ao Broker via MQTT. 
2. Ele **assina o tópico** `senai/dataJson`.
3. Ao receber os dados, o ESP32 analisa as informações para decidir:
   - Se a **temperatura estiver acima de 25ºC** → ativa o NeoPixel do **ar-condicionado** com a cor AZUL representando ar-condicionado ligado para **REFRIGERAR O AMBIEMNTE**.
   - Se a **temperatura estiver abaixo de 20ºC** → ativa o NeoPixel do **ar-condicionado** com a cor VERMELHO representando ar-condicionado ligado para **AQUECIMENTO DO AMBIENTE**.
   - Se a **luminosidade estiver baixa**  houver **presença detectada** → ativa os NeoPixels das **lâmpadas**.
   - A intensidade da iluminação das lâmpadas dependera da quantidade de **LUX** captada no ambiente.
   - Servos serão ativados quando a temperatura externa estiver entre 20ºC e 25ºC, representando a abertura das janelas para aproveitar a temperatura ambiente para climatizar a sala.
4. Os **NeoPixels acendem** conforme os estados determinados.

---

<a id="resumo-fluxo"></a>
## ✅ Resumo do Fluxo

```text
[Sensores] → coleta dados → publica no MQTT  
[Atuadores] ← recebe dados ← decide e aciona os NeoPixels e Servos


  
