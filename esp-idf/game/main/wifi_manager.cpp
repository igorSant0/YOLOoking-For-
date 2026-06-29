#include "wifi_manager.h"

#include "config.h"

static unsigned long lastReconnectAttempt = 0;

void connectWiFi() {
    Serial.println();
    Serial.println("==================================");
    Serial.println("YOLOuking - Conectando ao WiFi");
    Serial.println("==================================");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int tentativas = 0;

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");

        tentativas++;

        if (tentativas >= 40) {
            Serial.println();
            Serial.println("Falha ao conectar.");
            Serial.println("Tentando novamente...");

            WiFi.disconnect();
            delay(1000);
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

            tentativas = 0;
        }
    }

    Serial.println();
    Serial.println("WiFi conectado!");

    Serial.print("SSID: ");
    Serial.println(WIFI_SSID);

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
}

bool isWiFiConnected() { return WiFi.status() == WL_CONNECTED; }

void reconnectWiFi() {
    if (isWiFiConnected()) return;

    unsigned long agora = millis();

    if (agora - lastReconnectAttempt < 5000) return;

    lastReconnectAttempt = agora;

    Serial.println("Reconectando WiFi...");

    WiFi.disconnect();

    delay(500);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int timeout = 0;

    while (WiFi.status() != WL_CONNECTED && timeout < 20) {
        delay(500);
        Serial.print(".");
        timeout++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("Reconectado!");

        Serial.print("Novo IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println();
        Serial.println("Reconexão falhou.");
    }
}