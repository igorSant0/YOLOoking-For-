#ifndef GAME_H
#define GAME_H

#include <Arduino.h>

enum class GameState { IDLE, CONNECTED, RUNNING, HINT, SUCCESS, FAILED };

struct GameData {
    GameState state;
    String currentMessage;
    String currentHint;
    int score;
    // TODO: adicionar outros campos conforme o retorno do servidor
};

extern GameData gameData;

void updateGame();

void setGameState(GameState state);

#endif