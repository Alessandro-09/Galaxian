#include "Game.hpp"
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <cstdlib> 

Game::Game(ALLEGRO_FONT* font, int width, int height) 
    : font(font), width(width), height(height), 
      starSpeed(1.0f), speedMultiplier(1.0f), elapsedTime(0.0f) {  // Velocidad inicial 1.0
    generateStars();
}

void Game::generateStars() {
    stars.clear();
    for (int i = 0; i < 300; ++i) {
        stars.emplace_back(
            static_cast<float>(rand() % width),
            static_cast<float>(rand() % height),
            rand() % 3 // Tipo de estrella: 0=azul, 1=roja, 2=amarilla
        );
    }
}

void Game::update() {
    // Aumentar el tiempo transcurrido 
    elapsedTime += 1.0f/120.0f; 
    
    // AJUSTE DE VELOCIDAD (1): Velocidad inicial 1.0x, llega a 5x en 320 segundos
    speedMultiplier = 1.0f + std::min(elapsedTime / 80.0f, 4.0f); // Máximo 5x de velocidad
    
    for (auto& star : stars) {
        // AJUSTE DE VELOCIDAD (2): Velocidades base por tipo de estrella
        float speedBase;
        switch(star.type) {
            case 0: speedBase = 0.5f; break;  // Estrellas azules lentas
            case 1: speedBase = 1.0f; break;  // Estrellas rojas medias
            case 2: speedBase = 1.5f; break;  // Estrellas amarillas rápidas
        }
        
        star.y += speedBase * speedMultiplier;
        
        if (star.y > height) {
            star.y = 0;
            star.x = rand() % width;
        }
    }
}

void Game::draw() const {
    al_clear_to_color(al_map_rgb(5, 2, 10)); // Fondo oscuro del espacio
    float t = al_get_time(); // Tiempo actual para efectos de animación

    for (const auto& star : stars) {
        float twinkle = 0.6f + 0.4f * std::sin(t * 2 + star.x * 0.1f);  // Efecto de parpadeo
        
        if (star.type == 0) {
            // Estrellas azules
            al_draw_pixel(star.x, star.y, al_map_rgb(
                50 + 50 * twinkle,
                150 + 50 * twinkle,
                200 + 55 * twinkle
            ));
        } else if (star.type == 1) {
            // Estrellas rojas  
            al_draw_pixel(star.x, star.y, al_map_rgb(
                200 + 55 * twinkle,
                50 + 50 * twinkle,
                30 * twinkle
            ));
        } 
        else {
            // Estrellas amarillas
            al_draw_pixel(star.x, star.y, al_map_rgb(
                200 + 55 * twinkle,
                180 + 75 * twinkle,
                100 + 50 * twinkle
            ));
        }
    }

    al_flip_display();
}

int Game::run(SystemResources& sys) {
    ALLEGRO_EVENT event;
    bool running = true;

    al_start_timer(sys.timer);

    while (running) {
        al_wait_for_event(sys.eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false; 
        } 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
        } 
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            update();
            draw();
        }
    }

    return 0;
}
