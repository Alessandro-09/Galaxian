#include "Init.hpp"
#include <iostream>

// Esta función se encarga de inicializar todos los recursos principales del sistema:
// la ventana, la fuente, la cola de eventos y el temporizador.
// Si algo falla en el proceso, devuelve un struct con punteros nulos.
SystemResources initializeSystem(int width, int height, const char* fontPath, int fontSize) {
    SystemResources sys = { nullptr, nullptr, nullptr, nullptr };

    // Inicializa Allegro. Si falla, muestra un mensaje y retorna recursos vacíos.
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro.\n";
        return sys;
    }

    // Inicializa los módulos necesarios: teclado, fuentes, fuentes TTF y primitivas gráficas.
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // Crea la ventana principal del juego.
    sys.display = al_create_display(width, height);
    if (!sys.display) return sys;

    // Carga la fuente TTF que se usará en el juego.
    sys.font = al_load_ttf_font(fontPath, fontSize, 0);
    if (!sys.font) return sys;

    // Crea la cola de eventos y el temporizador para el bucle principal.
    sys.eventQueue = al_create_event_queue();
    sys.timer = al_create_timer(1.0 / 120); // 120 FPS

    // Si falla la creación de la cola de eventos o el temporizador, retorna recursos vacíos.
    if (!sys.eventQueue || !sys.timer) return sys;

    // Registra las fuentes de eventos: ventana, teclado y temporizador.
    al_register_event_source(sys.eventQueue, al_get_display_event_source(sys.display));
    al_register_event_source(sys.eventQueue, al_get_keyboard_event_source());
    al_register_event_source(sys.eventQueue, al_get_timer_event_source(sys.timer));

    // Si todo salió bien, retorna los recursos inicializados.
    return sys;
}

// Esta función libera todos los recursos del sistema para evitar fugas de memoria.
// Destruye la fuente, el temporizador, la cola de eventos y la ventana, si existen.
void cleanupSystem(SystemResources& sys) {
    if (sys.font) al_destroy_font(sys.font);
    if (sys.timer) al_destroy_timer(sys.timer);
    if (sys.eventQueue) al_destroy_event_queue(sys.eventQueue);
    if (sys.display) al_destroy_display(sys.display);
}
