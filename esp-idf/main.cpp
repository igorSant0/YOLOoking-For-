#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>

// Credenciais WiFi e servidor
const char *ssid = "MinhaRedeCasa";
const char *password = "senhaSuperSecreta";
const char *serverUrl = "http://192.168.0.10:5000";

// Botões
const int BTN_REGISTRO = 18;
const int BTN_VALIDACAO = 19;
const int BTN_DICA = 5;
const int BTN_RESET = 4;
const int BTN_naosei = 33;

// LEDs e buzzer
const int LED_VERDE = 25;
const int LED_VERMELHO = 26;
const int BUZZER = 27;

// LCD paralelo
LiquidCrystal lcd(21, 22, 14, 13, 32, 23);

void setup()
{
    Serial.begin(115200);

    lcd.begin(16, 2);
    lcd.print("Conectando...");

    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(BTN_REGISTRO, INPUT_PULLUP);
    pinMode(BTN_VALIDACAO, INPUT_PULLUP);
    pinMode(BTN_DICA, INPUT_PULLUP);
    pinMode(BTN_RESET, INPUT_PULLUP);
    pinMode(BTN_naosei, INPUT_PULLUP);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    lcd.clear();
    lcd.print("WiFi conectado!");
}

void loop()
{
    if (digitalRead(BTN_REGISTRO) == LOW)
        chamarRota("/game/connect");
    if (digitalRead(BTN_VALIDACAO) == LOW)
        chamarRota("/game/validate");
    if (digitalRead(BTN_DICA) == LOW)
        buscarDica();
    if (digitalRead(BTN_RESET) == LOW)
        chamarRota("/game/reset");
    if (digitalRead(BTN_naosei) == LOW)
        iniciarJogo(); // agora mostra a primeira dica

    delay(300);
}

// Função genérica para rotas simples
void chamarRota(String rota)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(String(serverUrl) + rota);
        int httpCode = http.GET();
        if (httpCode == 200)
        {
            String resposta = http.getString();
            Serial.println(resposta);

            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, resposta);

            if (!error)
            {
                const char *status = doc["status"];
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Status:");
                lcd.setCursor(0, 1);
                lcd.print(status);
                feedbackVisual(status);
            }
        }
        http.end();
    }
}

// Função para rota de dica
void buscarDica()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(String(serverUrl) + "/game/next-hint");
        int httpCode = http.GET();

        if (httpCode == 200)
        {
            String payload = http.getString();
            Serial.println(payload);

            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error)
            {
                const char *hint = doc["hint"];
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Dica:");
                lcd.setCursor(0, 1);
                lcd.print(hint);
            }
        }
        http.end();
    }
}

// Função para iniciar jogo e mostrar primeira dica
void iniciarJogo()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(String(serverUrl) + "/game/init");
        int httpCode = http.GET();

        if (httpCode == 200)
        {
            String payload = http.getString();
            Serial.println(payload);

            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (!error)
            {
                const char *status = doc["status"];
                const char *hint = doc["hint"];

                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Status:");
                lcd.print(status);
                lcd.setCursor(0, 1);
                lcd.print(hint); // mostra a primeira dica
            }
        }
        http.end();
    }
}

// Feedback visual com LEDs e buzzer
void feedbackVisual(String status)
{
    if (status == "CORRECT")
    {
        digitalWrite(LED_VERDE, HIGH);
        digitalWrite(LED_VERMELHO, LOW);
        tone(BUZZER, 1000, 200);
    }
    else if (status == "WRONG")
    {
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, HIGH);
        tone(BUZZER, 500, 400);
    }
    else if (status == "RESET")
    {
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        noTone(BUZZER);
    }
}
