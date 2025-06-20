#ifndef GAME_HPP
#define GAME_HPP

#include "Init.hpp"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <algorithm>
#include <random>
#include <string>

struct Star {
    float x;
    float y;
    int type;  
    
    Star(float x, float y, int type) : x(x), y(y), type(type) {}
};

class Game {
public:
    Game(ALLEGRO_FONT* font, int width, int height);
    ~Game(); 
    int run(SystemResources& sys);
    void draw() const;
    void dibujarenemigos() const;
    void dibujarnave() const;
    void dibujarbala() const;
    int getCurrentScore() const;

private:
    ALLEGRO_FONT* font;
    ALLEGRO_FONT* smallFont;
    int width, height;
    int currentScore;
    int highScore;

    // Variables para Game Over y explosión - REORDENADAS
    bool gameOver;
    bool showExplosion;
    float explosionTimer;
    float gameOverTimer;
    float starSpeed;
    float speedMultiplier; 
    float elapsedTime;  // MOVIDO DESPUÉS DE speedMultiplier
    
    ALLEGRO_BITMAP* explosionImg;
    ALLEGRO_BITMAP* livesSprite;

    std::vector<Star> stars;
    void crearnivel();
    void crearbala(int dy, int x, int y, SystemResources& sys);
    void colisiones(SystemResources& sys);
    void actualizarenemigos(SystemResources& sys);
    void actualizarbala(SystemResources& sys);
    void actualizarNave(SystemResources& sys);
    void crearnave();
    void generateStars();
    void update(SystemResources& sys);
};

#endif