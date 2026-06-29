#include "buttons.h"

#include <Arduino.h>
#include <esp_log.h>

#include "api.h"
#include "config.h"
#include "display.h"

static const char* TAG = "BTN";

static unsigned long lastPressTime[5] = {0};

static const uint8_t BUTTON_PINS[] = {BTN_CONNECT, BTN_START, BTN_VALIDATE, BTN_HINT, BTN_RESET};

static bool debounce(uint8_t index) {
    unsigned long now = millis();
    if (now - lastPressTime[index] < DEBOUNCE_TIME) return false;
    lastPressTime[index] = now;
    return true;
}

void initButtons() {
    for (uint8_t pin : BUTTON_PINS) {
        pinMode(pin, INPUT_PULLUP);
        ESP_LOGI(TAG, "Pino %d configurado como INPUT_PULLUP", pin);
    }
    ESP_LOGI(TAG, "Botoes inicializados.");
}

void checkButtons() {
    if (digitalRead(BTN_CONNECT) == LOW && debounce(0)) {
        ESP_LOGI(TAG, "BTN_CONNECT pressionado -> connectServer()");
        connectServer();
    }

    if (digitalRead(BTN_START) == LOW && debounce(1)) {
        ESP_LOGI(TAG, "BTN_START pressionado -> gameInit()");
        gameInit();
    }

    if (digitalRead(BTN_VALIDATE) == LOW && debounce(2)) {
        ESP_LOGI(TAG, "BTN_VALIDATE pressionado -> validateObject()");
        validateObject();
    }

    if (digitalRead(BTN_HINT) == LOW && debounce(3)) {
        ESP_LOGI(TAG, "BTN_HINT pressionado -> nextHint()");
        nextHint();
    }

    if (digitalRead(BTN_RESET) == LOW && debounce(4)) {
        ESP_LOGI(TAG, "BTN_RESET pressionado -> resetGame()");
        resetGame();
    }
}