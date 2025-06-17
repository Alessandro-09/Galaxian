#ifndef INSTRUCTIONS_SCREEN_HPP
#define INSTRUCTIONS_SCREEN_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include "Init.hpp"

class InstructionsScreen {
public:
    InstructionsScreen(ALLEGRO_FONT* font, int width, int height);
    ~InstructionsScreen();
    
    bool run(SystemResources& sys);
    void draw() const;

private:
    ALLEGRO_FONT* font;
    int width;
    int height;
    float loadingTime = 0.0f;      // Tiempo acumulado desde que se inició la pantalla
    bool showEnterText = false;    // Controla si se debe mostrar "PRESIONA ENTER..."
    mutable float pulseIntensity = 1.0f;  // Controla la intensidad del color (0.0 a 1.0)   

    // Declaración de los bitmaps
    ALLEGRO_BITMAP* arrows_img;
    ALLEGRO_BITMAP* space_img;
    ALLEGRO_BITMAP* esc_img;
    
    // Declaración de los métodos privados
    void loadAssets();
    void destroyAssets();
};

#endif