#include "Menu.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Constructor del menú, recibe la fuente, ancho y alto de la pantalla
Menu::Menu(ALLEGRO_FONT* font, int width, int height)
    : font(font), width(width), height(height), selectedOption(0)
{
    options = {
        "Start Game",
        "High Scores",
        "Exit"
    };

    // Semilla para posiciones consistentes de las estrellas de fondo
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Menu::~Menu() {}

// Procesa la entrada del teclado para navegar por el menú
void Menu::processInput(ALLEGRO_EVENT& event) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                // Selecciona la opción anterior (con ciclo)
                selectedOption = (selectedOption - 1 + options.size()) % options.size();
                break;
            case ALLEGRO_KEY_DOWN:
                // Selecciona la siguiente opción (con ciclo)
                selectedOption = (selectedOption + 1) % options.size();
                break;
        }
    }
}

// Devuelve la opción actualmente seleccionada
int Menu::getSelectedOption() const {
    return selectedOption;
}

// Dibuja el menú en pantalla
void Menu::draw() const {
    // Fondo azul oscuro, simulando el espacio
    al_clear_to_color(al_map_rgb(0, 0, 20)); // Azul muy oscuro

    // Dibuja estrellas estáticas de fondo
    static const int starCount = 100;
    static std::vector<std::pair<int, int>> stars;
    static bool initialized = false;

    if (!initialized) {
        for (int i = 0; i < starCount; ++i)
            stars.emplace_back(std::rand() % width, std::rand() % height);
        initialized = true;
    }

    float t = al_get_time();
    for (const auto& star : stars) {
        // Calcula el brillo de cada estrella con un efecto de parpadeo
        float brightness = 0.6f + 0.4f * std::sin(t * 4 + star.first);
        int val = static_cast<int>(brightness * 255);
        al_draw_pixel(star.first, star.second, al_map_rgb(val, val, val));
    }

    // Dibuja las opciones del menú
    for (size_t i = 0; i < options.size(); ++i) {
        // Efecto de pulso para el color (cambia el brillo con el tiempo)
        float pulse = 0.7f + 0.3f * std::sin(t * 4 + i);
    
        // Color base: Steel Blue 
        ALLEGRO_COLOR baseColor = al_map_rgb_f(0.27f * pulse, 0.51f * pulse, 0.71f * pulse);
    
        int x = width / 2;
        int y = 200 + static_cast<int>(i) * 60;
    
        // Sombra o resplandor del texto (efecto de brillo alrededor)
        for (int dx = -2; dx <= 2; ++dx) {
            for (int dy = -2; dy <= 2; ++dy) {
                if (dx != 0 || dy != 0) {
                    // Dibuja el texto con baja opacidad para crear el efecto de resplandor
                    al_draw_text(font, al_map_rgba_f(0.27f, 0.51f, 0.71f, 0.15f),
                                 x + dx, y + dy, ALLEGRO_ALIGN_CENTER, options[i].c_str());
                }
            }
        }
    
        // Si la opción está seleccionada, usa el color animado; si no, gris claro
        ALLEGRO_COLOR color = (i == selectedOption) ? baseColor : al_map_rgb(150, 150, 150);
    
        // Dibuja el texto principal de la opción
        al_draw_text(font, color, x, y, ALLEGRO_ALIGN_CENTER, options[i].c_str());
    
        /*
        // Si la opción está seleccionada, dibuja un triángulo marcador a la izquierda
        if (i == selectedOption) {
            float triSize = 10.0f;
            float px = x - 150;
            float py = y + 10;
    
            al_draw_filled_triangle(
                px,           py - triSize,
                px,           py + triSize,
                px + triSize, py,
                baseColor
            );
        }
        */
    }
    
    // Actualiza la pantalla
    al_flip_display();
}
