#ifndef CONFIG_H
#define CONFIG_H

// =========================
// WIFI
// =========================
#define WIFI_SSID "SEU_WIFI"
#define WIFI_PASSWORD "SUA_SENHA"

// TODO:
// Ajustar conforme o IP do backend Flask
#define SERVER_IP "192.168.0.100"
#define SERVER_PORT 5000

// =========================
// BOTÕES
// =========================
#define BTN_CONNECT 18
#define BTN_START 19
#define BTN_VALIDATE 4
#define BTN_HINT 5
#define BTN_RESET 33

// =========================
// SAÍDAS
// =========================
#define LED_GREEN 25
#define LED_RED 26
#define BUZZER 27

// =========================
// LCD
// =========================
#define LCD_RS 21
#define LCD_EN 22
#define LCD_D4 14
#define LCD_D5 13
#define LCD_D6 32
#define LCD_D7 23

// =========================
// DEBOUNCE
// =========================
#define DEBOUNCE_TIME 200

#endif