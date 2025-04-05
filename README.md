# 📡 Automatização e Controle Inteligente de Iluminação e Climatização do Ambiente

> Projeto desenvolvido nas aulas de **Internet das Coisas (IoT)** no **SENAI**, com o objetivo de automatizar o controle de lâmpadas e ar-condicionado, otimizando o consumo de energia. Através da conectividade e coleta de dados, o sistema monitora a iluminação e a temperatura do ambiente, permitindo uma gestão mais eficiente e sustentável dos recursos.

---

## 📌 Índice
- [Sobre o Projeto](#sobre-o-projeto)
- [Comunicação MQTT](#comunicacao-mqtt)
- [Módulo 1:Sensores](#modulo-1)
- [Módulo 2:Atuadores](#modulo-2)

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
1. O ESP32 lê os dados de todos os sensores.
2. O algoritmo então, irá mandar os dados iniciais e depois analisa se deve ou não enviar dados para o broker, verificando se houve alguma mudança em algum dado previamente coletado.
3. Os dados são formatados em JSON, como o exemplo abaixo.
   
**Exemplo de JSON criado:**
```json
{
  "movimento": true,
  "temperatura": 14,
  "umidade": 40,
  "luminosidade": 499.6338
}
```
4. Depois do JSON criado, ele é Serializado para poder enviar os dados em uma única String, como exemplo abaixo.
   
**Exemplo de payload JSON enviado:**
```json
{"movimento":true,"temperatura":14,"umidade":40,"luminosidade":499.6338}
```
5. O ESP32 publica os dados do JSON serializado no tópico `senai/dataJson` no broker MQTT.

---

<a id="modulo-2"></a>
## 💡 Módulo 2: Atuadores

Este módulo recebe os dados via MQTT e aciona os NeoPixels com base nas informações dos sensores.

### Atuadores utilizados:
- Tiras de LED **NeoPixel** (representando ar-condicionado e lâmpadas)

### Funcionamento:
1. O ESP32 se conecta ao mesmo broker MQTT.
2. Ele **assina o tópico** `senai/dataJson`.
3. Ao receber os dados, o ESP32 analisa as informações para decidir:
   - Se a **temperatura estiver alta** → ativa o NeoPixel do **ar-condicionado**.
   - Se a **luminosidade estiver baixa** ou houver **presença detectada** → ativa os NeoPixels das **lâmpadas**.
4. Os **NeoPixels acendem** conforme os estados determinados.

---

## ✅ Resumo do Fluxo

```text
[Sensores] → coleta dados → publica no MQTT  
[Atuadores] ← recebe dados ← decide e aciona os NeoPixels


  
