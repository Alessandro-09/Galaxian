#include "Menu.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Init.hpp"

// El constructor del menú recibe la fuente, el ancho y el alto de la pantalla.
// Inicializa las opciones del menú y genera las estrellas de fondo.
Menu::Menu(ALLEGRO_FONT* font, int width, int height)
    : font(font), width(width), height(height), selectedOption(0)
{
    options = {
        "Start Game",
        "High Scores",
        "Exit"
    };

    // Se inicializa la semilla para que las estrellas tengan posiciones aleatorias.
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Se generan las estrellas solo una vez al crear el menú.
    generateStars();
}

Menu::~Menu() {}

// Esta función genera 100 estrellas en posiciones aleatorias para el fondo.
void Menu::generateStars() {
    stars.clear();
    const int starCount = 100;
    for (int i = 0; i < starCount; ++i) {
        stars.emplace_back(std::rand() % width, std::rand() % height);
    }
}

// Esta función procesa la entrada del teclado para moverse por el menú.
// Si se presiona la flecha arriba o abajo, cambia la opción seleccionada.
void Menu::processInput(ALLEGRO_EVENT& event) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                selectedOption = (selectedOption - 1 + options.size()) % options.size();
                break;
            case ALLEGRO_KEY_DOWN:
                selectedOption = (selectedOption + 1) % options.size();
                break;
        }
    }
}

// Devuelve el índice de la opción actualmente seleccionada.
int Menu::getSelectedOption() const {
    return selectedOption;
}

// Esta función dibuja todo el menú en pantalla.
void Menu::draw() const {
    // Primero se limpia la pantalla con un color azul muy oscuro, simulando el espacio.
    al_clear_to_color(al_map_rgb(0, 0, 20)); // Azul muy oscuro

    float t = al_get_time();

    // Se dibujan las estrellas de fondo, con un efecto de parpadeo usando una función seno.
    for (const auto& star : stars) {
        float brightness = 0.6f + 0.4f * std::sin(t * 4 + star.first);
        int val = static_cast<int>(brightness * 255);
        al_draw_pixel(star.first, star.second, al_map_rgb(val, val, val));
    }

    // Ahora se dibujan las opciones del menú.
    for (size_t i = 0; i < options.size(); ++i) {
        // Se calcula un valor de "pulso" para animar el color de la opción seleccionada.
        float pulse = 0.7f + 0.3f * std::sin(t * 4 + i);

        // El color base es un azul claro (tipo "steel blue") que varía su brillo con el pulso.
        ALLEGRO_COLOR baseColor = al_map_rgb_f(0.27f * pulse, 0.51f * pulse, 0.71f * pulse);

        int x = width / 2;
        int y = 200 + static_cast<int>(i) * 60;

        // Se dibuja una sombra o resplandor alrededor del texto para darle un efecto de brillo.
        for (int dx = -2; dx <= 2; ++dx) {
            for (int dy = -2; dy <= 2; ++dy) {
                if (dx != 0 || dy != 0) {
                    al_draw_text(font, al_map_rgba_f(0.27f, 0.51f, 0.71f, 0.15f),
                                 x + dx, y + dy, ALLEGRO_ALIGN_CENTER, options[i].c_str());
                }
            }
        }

        // Si la opción está seleccionada, se usa el color animado; si no, un gris claro.
        ALLEGRO_COLOR color = (i == selectedOption) ? baseColor : al_map_rgb(150, 150, 150);

        // Finalmente, se dibuja el texto de la opción.
        al_draw_text(font, color, x, y, ALLEGRO_ALIGN_CENTER, options[i].c_str());
    }

    // Se actualiza la pantalla para mostrar los cambios.
    al_flip_display();
}

// Esta función ejecuta el bucle principal del menú.
// Espera eventos y responde a teclas o al cierre de la ventana.
int Menu::run(SystemResources& sys) {
    al_start_timer(sys.timer);
    bool running = true;
    int selected = -1;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(sys.eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            draw();
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            selected = 2;  // Si se cierra la ventana, se selecciona "Exit".
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                selected = getSelectedOption();
                running = false;
            } else {
                processInput(event);
            }
        }
    }

    return selected;
}
