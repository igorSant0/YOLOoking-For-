# Projeto ESP32 - Interface Física do Jogo YOLOoking-For

Este projeto implementa o firmware para o **ESP32** que funciona como interface física do jogo YOLOoking-For.  
O ESP32 controla **LCD, LEDs, buzzer e botões**, e se comunica com o servidor Flask através de rotas HTTP.

---

## 📌 Suporte ao ESP32

Antes de começar, é necessário instalar o suporte oficial ao ESP32 no Arduino IDE:

1. Abra o **Arduino IDE**.
2. Vá em **Ferramentas → Placa → Gerenciador de Placas**.
3. Procure por **esp32 by Espressif Systems**.
4. Clique em **Instalar**.

Isso adiciona as placas ESP32 e as bibliotecas básicas (`WiFi.h`, `HTTPClient.h`, etc).

---

## 📌 Bibliotecas necessárias

- **WiFi**  
  Já vem junto com o pacote do ESP32. Usada para conectar à rede.

- **HTTPClient**  
  Também já incluída no pacote ESP32. Permite fazer requisições GET/POST.

- **LiquidCrystal**  
  Para controlar LCD em modo paralelo.  
  Instale pelo _Gerenciador de Bibliotecas_ → procure por **LiquidCrystal**.

- **ArduinoJson**  
  Para parsear as respostas JSON do servidor e extrair campos como `"status"` e `"hint"`.  
  Instale pelo _Gerenciador de Bibliotecas_ → procure por **ArduinoJson by Benoit Blanchon**.

---

## 📌 Passo a passo para gravar o código na placa ESP32

1. **Conecte o ESP32** ao computador via cabo USB.
2. Abra o **Arduino IDE**.
3. Vá em **Ferramentas → Placa** e selecione **ESP32 Dev Module** (ou o modelo correspondente).
4. Vá em **Ferramentas → Porta** e selecione a porta serial correta (ex: COM3 no Windows, `/dev/ttyUSB0` no Linux).
5. Copie o código do projeto para o editor do Arduino IDE.
6. Clique em **Verificar** (ícone de ✔) para compilar e checar erros.
7. Clique em **Carregar** (ícone de →) para gravar o código na placa.
8. Abra o **Monitor Serial** (Ctrl+Shift+M) para acompanhar logs e depuração.

---

## 📌 Hardware utilizado

- ESP32 DevKit
- LCD paralelo (pinos configurados no código)
- LEDs (verde e vermelho)
- Buzzer
- Botões físicos

---

## 📌 Comunicação com servidor

O ESP32 se comunica com o servidor Flask através das seguintes rotas:

- `/game/connect` → Conectar hardware
- `/game/init` → Iniciar jogo
- `/game/validate` → Validar tentativa
- `/game/next-hint` → Solicitar próxima dica
- `/game/reset` → Resetar jogo

---

## 📌 Autor

Projeto desenvolvido para integrar hardware ESP32 com servidor Flask no jogo YOLOoking-For.
