#include "display.h"

#include <Arduino.h>

#include "config.h"
#include "hd44780.h"

// Define a configuração física usando suas macros do config.h
static hd44780_t lcd = {
    .write_cb = NULL,  // ADICIONADO: Remove o aviso de membro faltando na inicialização
    .pins =
        {
            .rs = (gpio_num_t)LCD_RS,
            .e = (gpio_num_t)LCD_EN,
            .d4 = (gpio_num_t)LCD_D4,
            .d5 = (gpio_num_t)LCD_D5,
            .d6 = (gpio_num_t)LCD_D6,
            .d7 = (gpio_num_t)LCD_D7,
            .bl = HD44780_NOT_USED  // Sem pino de backlight controlado por software
        },
    .font = HD44780_FONT_5X8,
    .lines = 2,
    .backlight = false};

void initDisplay() {
    hd44780_init(&lcd);
    hd44780_clear(&lcd);
}

void lcdMessage(const char* line1, const char* line2) {
    hd44780_clear(&lcd);

    // Primeira linha
    hd44780_gotoxy(&lcd, 0, 0);
    hd44780_puts(&lcd, line1);

    // Segunda linha
    hd44780_gotoxy(&lcd, 0, 1);
    hd44780_puts(&lcd, line2);
}

void lcdMessage(String line1, String line2) { lcdMessage(line1.c_str(), line2.c_str()); }

void lcdClear() { hd44780_clear(&lcd); }
