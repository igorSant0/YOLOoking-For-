#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <LiquidCrystal.h>
#include <WiFi.h>

// WiFi
const char *ssid = "Monteiro";
const char *password = "Reiki191280";
const char *serverUrl = "http://192.168.15.15:5001";

// Botões
const int BTN_REGISTRO = 18;
const int BTN_VALIDACAO = 19;
const int BTN_DICA = 5;
const int BTN_RESET = 4;
const int BTN_INIT = 33;

// LEDs e buzzer
const int LED_VERDE = 25;
const int LED_VERMELHO = 26;
const int BUZZER = 27;

// Variáveis para rastrear o estado anterior dos botões (Evita metralhadora de requisições)
bool lastBtnRegistro = HIGH;
bool lastBtnValidacao = HIGH;
bool lastBtnDica = HIGH;
bool lastBtnReset = HIGH;
bool lastBtnInit = HIGH;

// Variáveis de controle do LCD para evitar cintilação
unsigned long tempoUltimaAcao = 0;
bool telaLivre = true;

// LCD paralelo
LiquidCrystal lcd(21, 22, 14, 13, 32, 23);

void setup() {
    Serial.begin(115200);

    lcd.begin(16, 2);
    lcd.print("Conectando...");
    Serial.println("Conectando ao WiFi...");

    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, LOW);

    pinMode(BTN_REGISTRO, INPUT_PULLUP);
    pinMode(BTN_VALIDACAO, INPUT_PULLUP);
    pinMode(BTN_DICA, INPUT_PULLUP);
    pinMode(BTN_RESET, INPUT_PULLUP);
    pinMode(BTN_INIT, INPUT_PULLUP);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    lcd.clear();
    lcd.print("WiFi conectado!");
    Serial.println("\nWiFi conectado!");
    delay(1000);

    // --- NOVA LÓGICA DE RECONEXÃO COM O SERVIDOR ---
    Serial.println("Buscando o servidor do jogo...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando ao");
    lcd.setCursor(0, 1);
    lcd.print("server...");

    bool jogoConectado = false;

    // O ESP32 fica preso aqui até o Flask responder com HTTP 200
    while (!jogoConectado) {
        jogoConectado = chamarRota("/game/connect");

        if (!jogoConectado) {
            Serial.println("Servidor offline. Tentando de novo em 3s...");
            // A função chamarRota já imprimiu o erro no LCD.
            // Esperamos 3 segundos para o usuário conseguir ler a falha.
            delay(3000);

            // Volta para a tela de busca para a próxima tentativa
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Conectando ao");
            lcd.setCursor(0, 1);
            lcd.print("server...");
        }
    }

    // Se saiu do while, a conexão foi um sucesso!
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conexao");
    lcd.setCursor(0, 1);
    lcd.print("realizada!");
    delay(2000);  // Dá 2 segundos para o usuário comemorar

    lcd.clear();
    lcd.print("Pronto para uso");
    telaLivre = true;
    // -----------------------------------------------
}

void loop() {
    // 1. Leitura atual
    bool currentBtnRegistro = digitalRead(BTN_REGISTRO);
    bool currentBtnValidacao = digitalRead(BTN_VALIDACAO);
    bool currentBtnDica = digitalRead(BTN_DICA);
    bool currentBtnReset = digitalRead(BTN_RESET);
    bool currentBtnInit = digitalRead(BTN_INIT);

    // 2. Dispara apenas na MUDANÇA de estado (solto -> pressionado)
    if (lastBtnRegistro == HIGH && currentBtnRegistro == LOW) {
        telaLivre = false;
        Serial.println("Botão de registro pressionado");
        chamarRota("/game/connect");
        tempoUltimaAcao = millis();
    }
    if (lastBtnValidacao == HIGH && currentBtnValidacao == LOW) {
        telaLivre = false;
        Serial.println("Botão de validação pressionado");
        chamarRota("/game/validate");
        tempoUltimaAcao = millis();
    }
    if (lastBtnDica == HIGH && currentBtnDica == LOW) {
        telaLivre = false;
        Serial.println("Botão de dica pressionado");
        buscarDica();
        tempoUltimaAcao = millis();
    }
    if (lastBtnReset == HIGH && currentBtnReset == LOW) {
        telaLivre = false;
        Serial.println("Botão de reset pressionado");
        chamarRota("/game/reset");
        tempoUltimaAcao = millis();
    }
    if (lastBtnInit == HIGH && currentBtnInit == LOW) {
        telaLivre = false;
        Serial.println("Botão de inicialização pressionado");
        iniciarJogo();
        tempoUltimaAcao = millis();
    }

    // 3. Atualiza os estados
    lastBtnRegistro = currentBtnRegistro;
    lastBtnValidacao = currentBtnValidacao;
    lastBtnDica = currentBtnDica;
    lastBtnReset = currentBtnReset;
    lastBtnInit = currentBtnInit;

    // 4. Limpa o display apenas se passou 3 segundos da última ação
    if (!telaLivre && (millis() - tempoUltimaAcao > 3000)) {
        lcd.clear();
        lcd.print("Pronto para uso");
        telaLivre = true;

        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, LOW);
    }

    delay(50);  // Debounce elétrico
}

// Retorna bool para o loop do setup saber se a requisição deu certo
bool chamarRota(String rota) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(serverUrl) + rota);
        int httpCode = http.GET();

        if (httpCode == 200) {
            String resposta = http.getString();
            Serial.println(resposta);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, resposta);

            if (!error) {
                const char *status = doc["status"];
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Status:");
                lcd.setCursor(0, 1);
                lcd.print(status);
                Serial.print("Status recebido: ");
                Serial.println(status);
                feedbackVisual(status);
            }
            http.end();
            return true;  // Sucesso na requisição
        } else {
            Serial.println("Erro HTTP: " + String(httpCode));
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Erro: " + String(httpCode));
            http.end();
            return false;  // Falha na requisição
        }
    }
    return false;  // Falha por falta de WiFi
}

void buscarDica() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(serverUrl) + "/game/next-hint");
        int httpCode = http.GET();

        if (httpCode == 200) {
            String payload = http.getString();
            Serial.println(payload);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                const char *hint = doc["hint"];
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Dica:");
                lcd.setCursor(0, 1);
                lcd.print(hint);
                Serial.print("Dica recebida: ");
                Serial.println(hint);
            }
        }
        http.end();
    }
}

void iniciarJogo() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(serverUrl) + "/game/init");
        int httpCode = http.GET();

        if (httpCode == 200) {
            String payload = http.getString();
            Serial.println(payload);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                const char *status = doc["status"];
                const char *hint = doc["hint"];

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Status: ");
                lcd.print(status);
                lcd.setCursor(0, 1);
                lcd.print(hint);
                Serial.println("Jogo iniciado com sucesso!");
                Serial.print("Dica inicial: ");
                Serial.println(hint);
            }
        }
        if (httpCode != 200) {
            Serial.print("Erro ao iniciar o jogo. Codigo HTTP: ");
            Serial.println(httpCode);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Erro Init: ");
            lcd.setCursor(0, 1);
            lcd.print(String(httpCode));
        }
        http.end();
    }
}

void feedbackVisual(String status) {
    if (status == "CORRECT") {
        Serial.println("Feedback visual: Correto");
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_VERMELHO, LOW);
        tone(BUZZER, 1000, 200);
    } else if (status == "WRONG") {
        Serial.println("Feedback visual: Incorreto");
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, HIGH);
        tone(BUZZER, 500, 400);
    } else if (status == "RESET") {
        Serial.println("Feedback visual: Reset");
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        noTone(BUZZER);
    }
}