// src/main.cpp
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "Menu.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
    if (!al_init()) return -1;
    al_install_keyboard();
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
    if (!display) return -1;

    ALLEGRO_EVENT_QUEUE* eventQueue = al_create_event_queue();
    if (!eventQueue) return -1;

    ALLEGRO_FONT* font = al_load_ttf_font("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 32, 0);
    if (!font) return -1;

    al_register_event_source(eventQueue, al_get_display_event_source(display));
    al_register_event_source(eventQueue, al_get_keyboard_event_source());

    Menu menu(font, WIDTH, HEIGHT);

    bool running = true;
    bool enterPressed = false;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                enterPressed = true;
            } else {
                menu.processInput(event);
            }
        }

        menu.draw();

        if (enterPressed) {
            int selected = menu.getSelectedOption();
            if (selected == 2) { // opcion "Exit"
                running = false;
            } else {
                // Aqui van las demas opciones del menu
                running = false;
            }
        }
    }

    al_destroy_font(font);
    al_destroy_event_queue(eventQueue);
    al_destroy_display(display);
    return 0;
}
