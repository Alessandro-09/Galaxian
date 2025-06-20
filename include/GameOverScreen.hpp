#ifndef GAME_OVER_SCREEN_HPP
#define GAME_OVER_SCREEN_HPP

#include <string>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "Init.hpp"

class GameOverScreen {
public:
    GameOverScreen(ALLEGRO_FONT* font, int width, int height);

    // Muestra la pantalla final, captura el nombre y lo devuelve
    std::string show(SystemResources& sys, int score);

private:
    ALLEGRO_FONT* font;
    int screenWidth;
    int screenHeight;
    ALLEGRO_SAMPLE* sample;
    ALLEGRO_SAMPLE_INSTANCE* instance;
};

#endif // GAME_OVER_SCREEN_HPP