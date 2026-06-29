#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal.h>

// WiFi
const char *ssid = "SEU_WIFI"; // nome da rede WiFi usada
const char *password = "SENHA_WIFI";
const char *serverUrl = "http://IP_DO_SERVIDOR:PORTA"; // endereço IP e porta do servidor Flask que esta rodando o backend

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

// LCD paralelo
LiquidCrystal lcd(21, 22, 14, 13, 32, 23);

void setup()
{
    Serial.begin(115200);

    lcd.begin(16, 2);
    lcd.print("Conectando...");
    Serial.println("Conectando ao WiFi...");

    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_VERMELHO, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(BTN_REGISTRO, INPUT_PULLUP);
    pinMode(BTN_VALIDACAO, INPUT_PULLUP);
    pinMode(BTN_DICA, INPUT_PULLUP);
    pinMode(BTN_RESET, INPUT_PULLUP);
    pinMode(BTN_INIT, INPUT_PULLUP);

    lcd.print("Pinos configurados...");
    Serial.println("Pinos configurados...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    lcd.clear();
    lcd.print("WiFi conectado!");
    Serial.println("WiFi conectado!");
}

void loop()
{
    if (digitalRead(BTN_REGISTRO) == LOW)
    {
        Serial.println("Botão de registro pressionado");
        chamarRota("/game/connect");
        Serial.println("Chamei rota de registro");
    }
    if (digitalRead(BTN_VALIDACAO) == LOW)
    {
        Serial.println("Botão de validação pressionado");
        chamarRota("/game/validate");
        Serial.println("Chamei rota de validação");
    }
    if (digitalRead(BTN_DICA) == LOW)
    {
        Serial.println("Botão de dica pressionado");
        chamarRota("/game/next-hint");
        Serial.println("Chamei rota de dica");
    }
    if (digitalRead(BTN_RESET) == LOW)
    {
        Serial.println("Botão de reset pressionado");
        chamarRota("/game/reset");
        Serial.println("Chamei rota de reset");
    }
    if (digitalRead(BTN_INIT) == LOW)
    {
        Serial.println("Botão de inicialização pressionado");
        chamarRota("/game/init");
        Serial.println("Chamei rota de inicialização");
    }

    delay(300);
}

void chamarRota(String rota)
{
    Serial.println("Chamando rota: " + rota);
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(String(serverUrl) + rota);
        int httpCode = http.GET();
        if (httpCode == 200)
        {
            String resposta = http.getString();
            Serial.println(resposta);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Resp:");
            lcd.setCursor(0, 1);
            lcd.print(resposta.substring(0, 16)); // mostra parte da resposta
            feedbackVisual(resposta);
        }
        http.end();
    }
}

void feedbackVisual(String resposta)
{
    if (resposta.indexOf("CORRECT") >= 0)
    {
        Serial.println("Resposta correta");
        digitalWrite(LED_VERDE, HIGH);
        tone(BUZZER, 1000, 200);
    }
    else if (resposta.indexOf("WRONG") >= 0)
    {
        Serial.println("Resposta incorreta");
        digitalWrite(LED_VERMELHO, HIGH);
        tone(BUZZER, 500, 400);
    }
    else if (resposta.indexOf("RESET") >= 0)
    {
        Serial.println("Resposta de reset");
        digitalWrite(LED_VERDE, LOW);
        digitalWrite(LED_VERMELHO, LOW);
        noTone(BUZZER);
    }
}
