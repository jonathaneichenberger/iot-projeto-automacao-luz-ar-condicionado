# 📡 Automatização e Controle Inteligente de Iluminação e Climatização do Ambiente

> Projeto desenvolvido nas aulas de **Internet das Coisas (IoT)** no **SENAI**, com o objetivo de automatizar o controle de lâmpadas e ar-condicionado, otimizando o consumo de energia. Através da conectividade e coleta de dados, o sistema monitora a iluminação e a temperatura do ambiente, permitindo uma gestão mais eficiente e sustentável dos recursos.

---

## 📌 Índice
- [Sobre o Projeto](#sobre-o-projeto)
- [Comunicação MQTT](#comunicacao-mqtt)
- [Módulo 1:Sensores](#modulo-1)
- [Módulo 2:Atuadores](#modulo-2)
- [Resumo do Fluxo](#resumo-fluxo)
- [Configuração do Ambiente](#configuração-do-ambiente)
- [Rodando o Projeto](#rodando-o-projeto)
- [Tecnologias Utilizadas](#tecnologias-utilizadas)

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
```

---

<a id="configuração-do-ambiente"></a>
## 🛠 Configuração do Ambiente

1. Instale o [VS Code](https://code.visualstudio.com/)
2. Instale as extensões **Wokwi Simulator** e **PlatformIO**.  

    ![Extensão Wokwi](https://github.com/user-attachments/assets/3666f874-bbf5-4638-826c-ded0b49a78c4)  
    ![Extensão PlatformIO](https://github.com/user-attachments/assets/0f6cd5ce-d0f4-431e-87ce-f0d144ada96f)  

3. Para criar ou abrir um projeto no PlatformIO, siga os passos abaixo:  
   - **Abra a extensão do PlatformIO**:  
      ![PlatformIO](https://github.com/user-attachments/assets/895a8be8-e98c-47f8-bf7d-7f4b928bb168)  

   - **Aperte F1 e abra o PlatformIO Home**:  
      ![PlatformIO Home](https://github.com/user-attachments/assets/2ba931d2-86f7-4714-8836-f9c6c42ea66a)  

   - **Escolha "New Project" para criar um novo ou "Open Project" para abrir um existente**:  
      ![Novo Projeto](https://github.com/user-attachments/assets/0be7694b-f0b4-4688-90d8-714e2637f94a)  

   - **Configure o novo projeto** escolhendo a placa *Espressif ESP32 Dev Module*:  
      ![Configuração ESP32](https://github.com/user-attachments/assets/7599e8ca-0919-473d-b9ca-4e0af926e86c)  

4. **Montando o Circuito na Plataforma Wokwi**  
   - Acesse [Wokwi](https://wokwi.com/arduino), faça login e crie um novo projeto.  
   - Monte seu circuito eletrônico de acordo com o projeto.  
   - No Wokwi, copie o conteúdo do arquivo `diagram.json`:  
      ![Diagrama Wokwi](https://github.com/user-attachments/assets/00c177b2-f32d-4aa4-bcb8-3d6658b285c0)  
   - No VS Code, **crie um arquivo `diagram.json`** dentro da pasta do seu projeto e cole os dados copiados.  

5. **Criando o arquivo de configuração Wokwi**  
   - No VS Code, crie o arquivo `wokwi.toml` e adicione:  
     ```toml
     [wokwi]
     version = 1
     elf = ".pio/build/esp32dev/firmware.elf"
     firmware = ".pio/build/esp32dev/firmware.bin"
     ```

Agora é só programar no arquivo **src/main.cpp** e rodar a simulação! 🚀  

---

<a id="rodando-o-projeto"></a>
## ▶️ Rodando o Projeto

1. **Clone este repositório pelo Git Bash**:
   ```
   git clone https://github.com/jonathaneichenberger/iot-esp32-lighting-and-temperature-control-commercial-building-project.git
   ```
2. Acesse a Pasta do Projeto pelo Git Bash:
   ```
   cd iot-esp32-lighting-and-temperature-control-commercial-building-project
   ```
3. Abra o VS Code pelo Git Bash:
   ```
   code .
   ```
4. Use o comando ```CTRL + '``` para abrir o terminal do VS Code e para compilar use o comando:
   ```
   pio run
   ```
---

<a id="tecnologias-utilizadas"></a>
## 🔧 Tecnologias Utilizadas

Wokwi - Simulador de circuitos IoT

VS Code - Editor de código

PlatformIO - Ambiente de desenvolvimento para microcontroladores

ESP32 - Microcontrolador utilizado no projeto

ThingSpeak - Plataforma de análise e visualização de dados IoT

