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
    // Inicializa Allegro y recursos del sistema

    if (!al_init_image_addon()) { // Añade esta línea
        std::cerr << "Failed to initialize image addon!\n";
        return -1;
    }
    
    SystemResources sys = initializeSystem(SCREEN_WIDTH, SCREEN_HEIGHT, FONT_PATH, FONT_SIZE);

    if (!sys.display || !sys.font || !sys.eventQueue || !sys.timer) {
        std::cerr << "Initialization failed. Exiting.\n";
        cleanupSystem(sys);
        return -1;
    }

    // Ejecuta el menú y obtiene la opción seleccionada
    Menu menu(sys.font, SCREEN_WIDTH, SCREEN_HEIGHT);
    int option = menu.run(sys);

    // Maneja la opción seleccionada
    switch (option) {
        case 0: {  // Start Game
            InstructionsScreen instructions(sys.font, SCREEN_WIDTH, SCREEN_HEIGHT);
            if (instructions.run(sys)) { // Solo entra al juego si presiona ENTER
                Game game(sys.font, SCREEN_WIDTH, SCREEN_HEIGHT);
                game.run(sys);
            }
            break;
        }
        case 1:
            std::cout << "Mostrando puntuaciones...\n";
            // TODO: Mostrar puntuaciones aquí
            break;
        case 2:
            std::cout << "Saliendo del programa...\n";
            break;
        default:
            std::cerr << "Opción inválida.\n";
            break;
    }

    // Limpieza de recursos
    cleanupSystem(sys);
    return 0;
}
