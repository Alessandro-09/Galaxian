#include "Init.hpp"
#include "Menu.hpp"
#include <iostream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FONT_PATH "assets/space_font.ttf"
#define FONT_SIZE 36

int main() {
    // Inicializa Allegro y recursos del sistema
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
        case 0:
            std::cout << "Iniciando el juego...\n";
            // TODO: Lógica del juego aquí
            break;
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
