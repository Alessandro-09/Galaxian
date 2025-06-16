#ifndef INIT_HPP
#define INIT_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

struct SystemResources {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT* font;
    ALLEGRO_EVENT_QUEUE* eventQueue;
    ALLEGRO_TIMER* timer;
    int width;
    int height;
};

SystemResources initializeSystem(int width, int height, const char* fontPath, int fontSize);
void cleanupSystem(SystemResources& sys);

#endif
