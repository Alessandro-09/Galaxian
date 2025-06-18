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

void Menu::updateStars() {
    for (auto& star : stars) {
        star.second += 1.0f;  // velocidad constante hacia abajo

        if (star.second > height) {
            star.first = std::rand() % width;
            star.second = 0;  // reaparece arriba
        }
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
    // Fondo azul oscuro 
    al_clear_to_color(al_map_rgb(3, 5, 15)); 

    float t = al_get_time();

    // Estrellas
    for (const auto& star : stars) {
        float twinkle = 0.7f + 0.3f * std::sin(t * 3 + star.first * 10);
        if (static_cast<int>(star.first + star.second) % 3 == 0) {
            // Estrellas azules
            int blue = 150 + static_cast<int>(105 * twinkle);
            al_draw_pixel(star.first, star.second, al_map_rgb(50, 100, blue));
        } else {
            // Estrellas blancas 
            int val = 150 + static_cast<int>(105 * twinkle);
            al_draw_pixel(star.first, star.second, al_map_rgb(val, val, val + 30));
        }
    }

    // Opciones del menú con efecto
    for (size_t i = 0; i < options.size(); ++i) {
        float pulse = 0.9f + 0.1f * std::sin(t * 1.5f + i);

        // Gradiente azul-cyan para opción seleccionada
        ALLEGRO_COLOR selectedColor = al_map_rgb_f(
            0.2f * pulse, 
            0.5f * pulse, 
            0.8f + 0.2f * pulse
        );

        // Color gris para opciones no seleccionadas 
        ALLEGRO_COLOR normalColor = al_map_rgb(120, 150, 180);

        int x = width / 2;
        int y = 200 + static_cast<int>(i) * 60;

        // Efecto de resplandor
        if (i == selectedOption) {
            for (int glow = 1; glow <= 3; glow++) {
                float alpha = 0.3f / glow;
                al_draw_text(font, al_map_rgba_f(0.3f, 0.6f, 1.0f, alpha),
                            x, y, ALLEGRO_ALIGN_CENTER, options[i].c_str());
            }
        }

        // Sombra suave
        al_draw_text(font, al_map_rgba_f(0.05f, 0.05f, 0.1f, 0.5f),
                    x + 2, y + 2, ALLEGRO_ALIGN_CENTER, options[i].c_str());

        // Texto principal
        al_draw_text(font, (i == selectedOption) ? selectedColor : normalColor,
                    x, y, ALLEGRO_ALIGN_CENTER, options[i].c_str());

        // Pequeños indicadores para la opción seleccionada
        if (i == selectedOption) {
            float wingSize = 5.0f + 2.0f * std::sin(t * 3);
            al_draw_filled_triangle(
                x - 80, y + 10,
                x - 60 - wingSize, y + 10,
                x - 70, y + 10 - wingSize,
                al_map_rgb(100, 180, 255)
            );
            al_draw_filled_triangle(
                x + 80, y + 10,
                x + 60 + wingSize, y + 10,
                x + 70, y + 10 - wingSize,
                al_map_rgb(100, 180, 255)
            );
        }
    }

    al_flip_display();
}

// Esta función ejecuta el bucle principal del menú.
// Espera eventos y responde a teclas o al cierre de la ventana.
int Menu::run(SystemResources& sys) {
    al_start_timer(sys.timer);
    bool running = true;
    int selected = -1;

    if (sys.menuMusic && !al_get_audio_stream_playing(sys.menuMusic)) {
        al_set_audio_stream_playing(sys.menuMusic, true);
    }

    // Detener la música de instrucciones si está sonando
    if (sys.instructionsMusic && al_get_audio_stream_playing(sys.instructionsMusic)) {
        al_set_audio_stream_playing(sys.instructionsMusic, false);
    }

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(sys.eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            updateStars();
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
