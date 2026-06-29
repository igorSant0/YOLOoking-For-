#include <Arduino.h>
#include <WiFi.h>
#include <esp_log.h>

#include "api.h"
#include "buttons.h"
#include "config.h"
#include "display.h"
#include "game.h"
#include "outputs.h"
#include "wifi_manager.h"

static const char* TAG = "MAIN";

void setup() {
    Serial.begin(115200);

    ESP_LOGI(TAG, "Inicializando outputs...");
    initOutputs();

    ESP_LOGI(TAG, "Inicializando display...");
    initDisplay();

    ESP_LOGI(TAG, "Inicializando botoes...");
    initButtons();

    lcdMessage("YOLOuking", "Inicializando");

    ESP_LOGI(TAG, "Conectando ao WiFi...");
    connectWiFi();

    lcdMessage("WiFi", "Conectado");

    ESP_LOGI(TAG, "Conectando ao servidor...");
    connectServer();

    ESP_LOGI(TAG, "Setup concluido. Entrando no loop.");
}

void loop() {
    checkButtons();
    updateGame();
    delay(10);
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "Iniciando arduino");
    initArduino();
    ESP_LOGI(TAG, "Arduino concluido, chamando setup");
    setup();
    ESP_LOGI(TAG, "Setup concluido, entrando no loop");
    while (true) {
        loop();
    }
}