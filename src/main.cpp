#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include "Menu.hpp"
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro.\n";
        return -1;
    }

    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        std::cerr << "Failed to create display.\n";
        return -1;
    }

    ALLEGRO_FONT* font = al_load_ttf_font("assets/space_font.ttf", 28, 0);
    if (!font) {
        std::cerr << "Failed to load font.\n";
        return -1;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));

    Menu menu(font, SCREEN_WIDTH, SCREEN_HEIGHT);

    bool running = true;
    bool redraw = true;

    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        }

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            menu.processInput(event);

            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                switch (menu.getSelectedOption()) {
                    case 0:
                        std::cout << "Start Game selected.\n";
                        // TODO: Start game logic here
                        break;
                    case 1:
                        std::cout << "High Scores selected.\n";
                        // TODO: Show high scores
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

    return 0;
}
