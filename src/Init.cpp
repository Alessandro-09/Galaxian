#include "Init.hpp"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <iostream>

SystemResources initializeSystem(int width, int height, const char* fontPath, int fontSize) {
    SystemResources sys = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, width, height};

    // 1. Inicialización básica de Allegro
    if (!al_init()) {
        std::cerr << "Error al inicializar Allegro." << std::endl;
        return sys;
    }

    // 2. Inicializar addons esenciales
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // 3. Inicializar sistema de audio
    if (!al_install_audio()) {
        std::cerr << "Error al inicializar audio." << std::endl;
        return sys;
    }
    if (!al_init_acodec_addon()) {
        std::cerr << "Error al inicializar codecs de audio." << std::endl;
        return sys;
    }

    // 3.1. Inicializar mixer de audio
    if (!al_reserve_samples(8)) {
        std::cerr << "Error al inicializar mixer de audio." << std::endl;
        return sys;
    }

    // 4. Crear display
    sys.display = al_create_display(width, height);
    if (!sys.display) {
        std::cerr << "Error al crear display." << std::endl;
        return sys;
    }

    // 5. Cargar fuente
    sys.font = al_load_ttf_font(fontPath, fontSize, 0);
    if (!sys.font) {
        std::cerr << "Error cargando fuente: " << fontPath << std::endl;
        cleanupSystem(sys);
        return sys;
    }

    // 6. Configurar timer y event queue
    sys.timer = al_create_timer(1.0 / 60.0); // 120 FPS
    sys.eventQueue = al_create_event_queue();
    if (!sys.timer || !sys.eventQueue) {
        std::cerr << "Error al crear timer o event queue." << std::endl;
        cleanupSystem(sys);
        return sys;
    }

    // 7. Cargar músicas
    sys.menuMusic = al_load_audio_stream("assets/space_music.ogg", 4, 2048);
    sys.instructionsMusic = al_load_audio_stream("assets/instructions_music.ogg", 4, 2048);
    sys.shootSound = al_load_sample("assets/shoot.ogg");
    sys.hitEnemySound = al_load_sample("assets/hit_enemy.ogg");
    sys.hitPlayerSound = al_load_sample("assets/hit_player.ogg");


    if (!sys.menuMusic) {
        std::cerr << "Advertencia: No se pudo cargar la música del menú." << std::endl;
    } else {
        al_set_audio_stream_playmode(sys.menuMusic, ALLEGRO_PLAYMODE_LOOP);
        if (!al_attach_audio_stream_to_mixer(sys.menuMusic, al_get_default_mixer())) {
            al_destroy_audio_stream(sys.menuMusic);
            sys.menuMusic = nullptr;
        }
    }

    if (!sys.instructionsMusic) {
        std::cerr << "Advertencia: No se pudo cargar la música de instrucciones." << std::endl;
    } else {
        al_set_audio_stream_playmode(sys.instructionsMusic, ALLEGRO_PLAYMODE_LOOP);
        if (!al_attach_audio_stream_to_mixer(sys.instructionsMusic, al_get_default_mixer())) {
            al_destroy_audio_stream(sys.instructionsMusic);
            sys.instructionsMusic = nullptr;
        }
    }

    if (!sys.shootSound) {
        std::cerr << "Error al cargar shoot.ogg" << std::endl;
    }

    if (!sys.hitEnemySound) {
        std::cerr << "Error: No se pudo cargar hit_enemy.ogg." << std::endl;
    }

    if (!sys.hitPlayerSound) {
        std::cerr << "Error: No se pudo cargar hit_player.ogg." << std::endl;
    }

    // 8. Registrar fuentes de eventos
    al_register_event_source(sys.eventQueue, al_get_display_event_source(sys.display));
    al_register_event_source(sys.eventQueue, al_get_keyboard_event_source());
    al_register_event_source(sys.eventQueue, al_get_timer_event_source(sys.timer));

    return sys;
}

void cleanupSystem(SystemResources& sys) {
    // Liberar recursos 
    if (sys.menuMusic) {
        al_set_audio_stream_playing(sys.menuMusic, false);
        al_detach_audio_stream(sys.menuMusic);
        al_destroy_audio_stream(sys.menuMusic);
    }
    if (sys.instructionsMusic) {
        al_set_audio_stream_playing(sys.instructionsMusic, false);
        al_detach_audio_stream(sys.instructionsMusic);
        al_destroy_audio_stream(sys.instructionsMusic);
    }
    if (sys.shootSound) al_destroy_sample(sys.shootSound);
    if (sys.hitEnemySound) al_destroy_sample(sys.hitEnemySound);
    if (sys.hitPlayerSound) al_destroy_sample(sys.hitPlayerSound);
    
    if (sys.font) al_destroy_font(sys.font);
    if (sys.timer) al_destroy_timer(sys.timer);
    if (sys.eventQueue) al_destroy_event_queue(sys.eventQueue);
    if (sys.display) al_destroy_display(sys.display);
    
    // Cierre seguro del sistema de audio
    al_uninstall_audio();
}