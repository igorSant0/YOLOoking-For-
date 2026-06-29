#include "game.h"

#include <esp_log.h>

#include "display.h"
#include "outputs.h"

static const char* TAG = "GAME";

GameData gameData = {GameState::IDLE, "", "", 0};

void updateGame() {
    // TODO: implementar lógica de atualização periódica se necessário
    //       (ex: polling de status, timeout de rodada, animações no LCD)
}

void setGameState(GameState state) {
    gameData.state = state;

    switch (state) {
        case GameState::IDLE:
            ESP_LOGI(TAG, "Estado: IDLE");
            turnOffOutputs();
            lcdMessage("YOLOuking", "Aguardando...");
            break;

        case GameState::CONNECTED:
            ESP_LOGI(TAG, "Estado: CONNECTED");
            lcdMessage("Servidor", "Conectado!");
            break;

        case GameState::RUNNING:
            ESP_LOGI(TAG, "Estado: RUNNING");
            lcdMessage("Jogo", "Em andamento");
            break;

        case GameState::HINT:
            ESP_LOGI(TAG, "Estado: HINT | hint: \"%s\"", gameData.currentHint.c_str());
            lcdMessage("Dica:", gameData.currentHint);
            break;

        case GameState::SUCCESS:
            ESP_LOGI(TAG, "Estado: SUCCESS | message: \"%s\"", gameData.currentMessage.c_str());
            greenBlink();
            beepVictory();
            lcdMessage("Correto!", gameData.currentMessage.c_str());
            break;

        case GameState::FAILED:
            ESP_LOGW(TAG, "Estado: FAILED | message: \"%s\"", gameData.currentMessage.c_str());
            redBlink();
            beepError();
            lcdMessage("Errado!", gameData.currentMessage.c_str());
            break;
    }
}