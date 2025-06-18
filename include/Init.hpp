#ifndef INIT_HPP
#define INIT_HPP

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>    
#include <allegro5/allegro_acodec.h>   
#include <allegro5/allegro_primitives.h>

struct SystemResources {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT* font;
    ALLEGRO_EVENT_QUEUE* eventQueue;
    ALLEGRO_TIMER* timer;
    ALLEGRO_AUDIO_STREAM* menuMusic;       // Música para el menú principal
    ALLEGRO_AUDIO_STREAM* instructionsMusic; // Música para la pantalla de instrucciones       
    
    int width;
    int height;
};

// Inicializa Allegro y carga recursos
SystemResources initializeSystem(int width, int height, const char* fontPath, int fontSize);

// Libera todos los recursos
void cleanupSystem(SystemResources& sys);

#endif


#include <allegro5/allegro_primitives.h>