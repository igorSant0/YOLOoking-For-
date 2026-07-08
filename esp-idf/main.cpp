#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <LiquidCrystal.h>
#include <WiFi.h>

// WiFi
const char *ssid = "TP-LINK_7FBE";
const char *password = "34637043";
const char *serverUrl = "http://192.168.0.106:5001";

// Botões
const int BTN_SCORE = 18;
const int BTN_VALIDACAO = 19;
const int BTN_DICA = 5;
const int BTN_RESET = 4;
const int BTN_INIT = 33;

// LEDs e buzzer
const int LED_VERDE = 25;
const int LED_VERMELHO = 26;
const int BUZZER = 27;

// Variáveis para rastrear o estado anterior dos botões
bool lastBtnScore = HIGH;
bool lastBtnValidacao = HIGH;
bool lastBtnDica = HIGH;
bool lastBtnReset = HIGH;
bool lastBtnInit = HIGH;

// Variáveis de controle de interface
unsigned long tempoUltimaAcao = 0;
unsigned long tempoExibicao = 2000;
bool telaLivre = true;
bool jogoIniciado = false;
String ultimaDica = "";

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

    pinMode(BTN_SCORE, INPUT_PULLUP);
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

    // LÓGICA DE RECONEXÃO COM O SERVIDOR
    Serial.println("Buscando o servidor do jogo...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando ao");
    lcd.setCursor(0, 1);
    lcd.print("server...");

    bool jogoConectado = false;

    while (!jogoConectado) {
        jogoConectado = chamarRota("/game/connect");

        if (!jogoConectado) {
            Serial.println("Servidor offline. Tentando de novo em 3s...");
            delay(3000);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Conectando ao");
            lcd.setCursor(0, 1);
            lcd.print("server...");
        }
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conexao");
    lcd.setCursor(0, 1);
    lcd.print("realizada!");
    delay(2000);

    lcd.clear();
    lcd.print("Pronto para uso");
    telaLivre = true;
    jogoIniciado = false;
}

void loop() {
    bool currentBtnScore = digitalRead(BTN_SCORE);
    bool currentBtnValidacao = digitalRead(BTN_VALIDACAO);
    bool currentBtnDica = digitalRead(BTN_DICA);
    bool currentBtnReset = digitalRead(BTN_RESET);
    bool currentBtnInit = digitalRead(BTN_INIT);

    if (lastBtnScore == HIGH && currentBtnScore == LOW) {
        Serial.println("Botão de score pressionado");
        buscarScore();
        tempoExibicao = 7000;
        telaLivre = false;
        tempoUltimaAcao = millis();
    }

    if (lastBtnValidacao == HIGH && currentBtnValidacao == LOW) {
        Serial.println("Botão de validação pressionado");
        chamarRota("/game/validate");
        tempoExibicao = 3000;
        telaLivre = false;
        tempoUltimaAcao = millis();
    }

    if (lastBtnDica == HIGH && currentBtnDica == LOW) {
        Serial.println("Botão de dica pressionado");
        buscarDica();
    }

    if (lastBtnReset == HIGH && currentBtnReset == LOW) {
        Serial.println("Botão de reset pressionado");
        resetarJogo();
    }

    if (lastBtnInit == HIGH && currentBtnInit == LOW) {
        Serial.println("Botão de inicialização pressionado");
        iniciarJogo();
        telaLivre = true;
    }

    lastBtnScore = currentBtnScore;
    lastBtnValidacao = currentBtnValidacao;
    lastBtnDica = currentBtnDica;
    lastBtnReset = currentBtnReset;
    lastBtnInit = currentBtnInit;

    if (!telaLivre && (millis() - tempoUltimaAcao > tempoExibicao)) {
        lcd.clear();
        lcd.setCursor(0, 0);

        if (jogoIniciado) {
            lcd.print("Dica:");
            lcd.setCursor(0, 1);
            lcd.print(ultimaDica);
        } else {
            lcd.print("Pronto para uso");
        }

        telaLivre = true;

        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, LOW);
    }

    delay(50);
}

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

                if (String(status) == "CORRECT" && doc["hint"].is<const char *>()) {
                    const char *hint = doc["hint"];
                    ultimaDica = String(hint);
                }

                if (rota != "/game/connect") {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Status:");
                    lcd.setCursor(0, 1);
                    lcd.print(status);
                }

                Serial.print("Status recebido: ");
                Serial.println(status);
                feedbackVisual(status);
            }
            http.end();
            return true;
        } else {
            Serial.println("Erro HTTP: " + String(httpCode));
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Erro: " + String(httpCode));
            http.end();
            return false;
        }
    }
    return false;
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
                const char *status = doc["status"];

                if (String(status) == "EXHAUSTED") {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Sem mais dicas!");

                    feedbackVisual("EXHAUSTED");
                    telaLivre = false;
                    tempoExibicao = 2000;
                    tempoUltimaAcao = millis();
                } else {
                    const char *hint = doc["hint"];
                    ultimaDica = String(hint);

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Dica:");
                    lcd.setCursor(0, 1);
                    lcd.print(ultimaDica);

                    Serial.print("Dica recebida: ");
                    Serial.println(hint);

                    telaLivre = true;
                }
            }
        } else {
            lcd.clear();
            lcd.print("Erro Dica: " + String(httpCode));
            telaLivre = false;
            tempoExibicao = 3000;
            tempoUltimaAcao = millis();
        }
        http.end();
    }
}

void buscarScore() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(serverUrl) + "/game/score");
        int httpCode = http.GET();

        if (httpCode == 200) {
            String payload = http.getString();
            Serial.println(payload);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error) {
                int total_score = doc["total_score"];
                int current_level = doc["current_level"];
                int hints = doc["hints_requested"];

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Lvl:" + String(current_level) + " Pts:" + String(total_score));
                lcd.setCursor(0, 1);
                lcd.print("Dicas: " + String(hints));

                Serial.println("Score atualizado no LCD.");
            }
        } else {
            lcd.clear();
            lcd.print("Erro Score: " + String(httpCode));
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

                jogoIniciado = true;
                ultimaDica = String(hint);

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Dica: ");
                lcd.setCursor(0, 1);
                lcd.print(ultimaDica);

                Serial.println("Jogo iniciado com sucesso!");
                Serial.print("Status interno: ");
                Serial.println(status);
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

            telaLivre = false;
            tempoExibicao = 2000;
            tempoUltimaAcao = millis();
        }
        http.end();
    }
}

void resetarJogo() {
    if (WiFi.status() == WL_CONNECTED) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Resetando...");

        HTTPClient http;
        http.begin(String(serverUrl) + "/game/reset");
        int httpCode = http.GET();

        if (httpCode == 200) {
            Serial.println("Comando de reset aceito pelo servidor.");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Jogo Resetado!");
            feedbackVisual("RESET");
        } else {
            Serial.print("Falha ao resetar servidor. HTTP: ");
            Serial.println(httpCode);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Erro Reset: ");
            lcd.setCursor(0, 1);
            lcd.print(String(httpCode));
        }
        http.end();

        delay(1500);

        Serial.println("Reiniciando o sistema...");
        ESP.restart();
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
    } else if (status == "EXHAUSTED") {
        Serial.println("Feedback visual: Dicas esgotadas");
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, HIGH);
        tone(BUZZER, 300, 600);
    } else if (status == "RESET") {
        Serial.println("Feedback visual: Reset");
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        noTone(BUZZER);
    }
}