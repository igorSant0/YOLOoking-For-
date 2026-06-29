#include "api.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <esp_log.h>

#include "config.h"
#include "display.h"
#include "game.h"
#include "outputs.h"

static const char* TAG = "API";

// TODO: definir as rotas conforme o servidor Flask
#define ROUTE_CONNECT "/connect"
#define ROUTE_GAME_INIT "/game/init"
#define ROUTE_VALIDATE "/game/validate"
#define ROUTE_HINT "/game/hint"
#define ROUTE_RESET "/game/reset"

bool sendGET(String endpoint) {
    if (WiFi.status() != WL_CONNECTED) {
        ESP_LOGW(TAG, "sendGET(%s): sem WiFi", endpoint.c_str());
        lcdMessage("Erro", "Sem WiFi");
        return false;
    }

    WiFiClient client;
    HTTPClient http;

    String url = String("http://") + SERVER_IP + ":" + String(SERVER_PORT) + endpoint;

    ESP_LOGI(TAG, "GET %s", url.c_str());

    http.begin(client, url);

    int httpCode = http.GET();

    if (httpCode <= 0) {
        ESP_LOGE(TAG, "Erro HTTP: %d (%s)", httpCode, HTTPClient::errorToString(httpCode).c_str());
        lcdMessage("Servidor", "Offline");
        http.end();
        return false;
    }

    ESP_LOGI(TAG, "HTTP %d", httpCode);

    String payload = http.getString();

    ESP_LOGI(TAG, "Payload: %s", payload.c_str());

    JsonDocument doc;

    DeserializationError erro = deserializeJson(doc, payload);

    if (erro) {
        ESP_LOGE(TAG, "Falha ao parsear JSON: %s", erro.c_str());
        lcdMessage("Erro", "JSON");
        http.end();
        return false;
    }

    processServerResponse(doc);

    http.end();
    return true;
}

void processServerResponse(JsonDocument& doc) {
    // TODO: ajustar os campos conforme o contrato de resposta do servidor Flask
    //       Exemplo assumido: { "status": "ok|error|hint|success|failed", "message": "...", "hint":
    //       "..." }

    const char* status = doc["status"] | "";
    const char* message = doc["message"] | "";
    const char* hint = doc["hint"] | "";

    ESP_LOGI(TAG, "Resposta -> status: \"%s\" | message: \"%s\" | hint: \"%s\"", status, message,
             hint);

    gameData.currentMessage = String(message);
    gameData.currentHint = String(hint);

    if (strcmp(status, "ok") == 0) {
        setGameState(GameState::RUNNING);

    } else if (strcmp(status, "success") == 0) {
        setGameState(GameState::SUCCESS);

    } else if (strcmp(status, "failed") == 0) {
        setGameState(GameState::FAILED);

    } else if (strcmp(status, "hint") == 0) {
        setGameState(GameState::HINT);

    } else if (strcmp(status, "error") == 0) {
        ESP_LOGW(TAG, "Servidor retornou erro: %s", message);
        lcdMessage("Erro", message);
        beepError();

    } else {
        // TODO: tratar outros status que o servidor possa retornar
        ESP_LOGW(TAG, "Status desconhecido: \"%s\"", status);
    }
}

bool connectServer() {
    ESP_LOGI(TAG, "connectServer()");
    lcdMessage("Servidor", "Conectando...");
    bool ok = sendGET(ROUTE_CONNECT);
    if (ok) setGameState(GameState::CONNECTED);
    return ok;
}

bool gameInit() {
    ESP_LOGI(TAG, "gameInit()");
    lcdMessage("Iniciando", "Jogo...");
    return sendGET(ROUTE_GAME_INIT);
}

bool validateObject() {
    ESP_LOGI(TAG, "validateObject()");
    lcdMessage("Validando", "Objeto...");
    return sendGET(ROUTE_VALIDATE);
}

bool nextHint() {
    ESP_LOGI(TAG, "nextHint()");
    lcdMessage("Buscando", "Dica...");
    return sendGET(ROUTE_HINT);
}

bool resetGame() {
    ESP_LOGI(TAG, "resetGame()");
    lcdMessage("Resetando", "Jogo...");
    bool ok = sendGET(ROUTE_RESET);
    if (ok) setGameState(GameState::IDLE);
    return ok;
}