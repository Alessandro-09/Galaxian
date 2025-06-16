#include "Init.hpp"
#include "Menu.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <iostream>

void runApp(int width, int height, const char* fontPath, int fontSize) {
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro.\n";
        return;
    }

    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY* display = al_create_display(width, height);
    if (!display) {
        std::cerr << "Failed to create display.\n";
        return;
    }

    ALLEGRO_FONT* font = al_load_ttf_font(fontPath, fontSize, 0);
    if (!font) {
        std::cerr << "Failed to load font.\n";
        al_destroy_display(display);
        return;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);

    if (!queue || !timer) {
        std::cerr << "Failed to create event queue or timer.\n";
        if (queue) al_destroy_event_queue(queue);
        if (timer) al_destroy_timer(timer);
        al_destroy_font(font);
        al_destroy_display(display);
        return;
    }

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Menu menu(font, width, height);
    bool running = true;
    bool redraw = true;

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            menu.processInput(event);
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                switch (menu.getSelectedOption()) {
                    case 0:
                        std::cout << "Start Game selected.\n";
                        // TODO: Game logic
                        break;
                    case 1:
                        std::cout << "High Scores selected.\n";
                        // TODO: High score logic
                        break;
                    case 2:
                        running = false;
                        break;
                }
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            menu.draw();
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}
