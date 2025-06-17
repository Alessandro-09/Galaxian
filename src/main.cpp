#include "Init.hpp"
#include "Menu.hpp"
#include "Game.hpp"
#include "InstructionsScreen.hpp"
#include <allegro5/allegro_image.h>
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FONT_PATH "assets/space_font.ttf"
#define FONT_SIZE 36

int main() {
    // === 1. INICIALIZACIÓN ===
    if (!al_init()) {
        std::cerr << "Error: Fallo al inicializar Allegro.\n";
        return -1;
    }

    // Addons esenciales
    al_init_image_addon();
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // Audio (maneja errores sin crashear)
    bool audio_ok = al_install_audio() && al_init_acodec_addon();
    if (!audio_ok) {
        std::cerr << "Warning: Audio desactivado.\n";
    }

    // Carga recursos
    SystemResources sys = initializeSystem(SCREEN_WIDTH, SCREEN_HEIGHT, FONT_PATH, FONT_SIZE);
    if (!sys.display || !sys.font || !sys.eventQueue || !sys.timer) {
        std::cerr << "Error: Recursos críticos fallaron.\n";
        cleanupSystem(sys);
        return -1;
    }

    // Configura música
    if (sys.music) {
        al_set_audio_stream_playmode(sys.music, ALLEGRO_PLAYMODE_LOOP);
        al_set_audio_stream_gain(sys.music, 0.6f);  // 60% volumen
        al_set_audio_stream_playing(sys.music, true);
    }

    // === BUCLE PRINCIPAL ===
    bool running = true;
    while (running) {
        Menu menu(sys.font, SCREEN_WIDTH, SCREEN_HEIGHT);
        int option = menu.run(sys);

        switch (option) {
            case 0: {  // Start Game
                InstructionsScreen instructions(sys.font, SCREEN_WIDTH, SCREEN_HEIGHT);
                if (instructions.run(sys)) {
                    if (sys.music) al_set_audio_stream_gain(sys.music, 0.3f);  // Baja volumen en juego
                    
                    Game game(sys.font, SCREEN_WIDTH, SCREEN_HEIGHT);
                    game.run(sys);
                    
                    if (sys.music) al_set_audio_stream_gain(sys.music, 0.6f);  // Restaura volumen
                }
                break;
            }
            case 1:  // High Scores
                std::cout << "Mostrando puntuaciones...\n";


                // Lógica de puntuaciones


                break;
            case 2:  // Exit
                running = false;
                break;
            default:
                std::cerr << "Error: Opción inválida.\n";
                break;
        }
    }

    // === 3. LIMPIEZA ===
    cleanupSystem(sys);
    return 0;
}