#include "InstructionsScreen.hpp"
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <cmath>
#include <ctime>


// El constructor recibe la fuente y el tamaño de la pantalla.
// Inicializa los punteros de las imágenes a nulo y luego carga los recursos gráficos.
InstructionsScreen::InstructionsScreen(ALLEGRO_FONT* font, int width, int height) 
    : font(font), width(width), height(height) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    arrows_img = nullptr;
    space_img = nullptr;
    esc_img = nullptr;
    loadAssets();
    generateStars();
}

// El destructor se asegura de liberar la memoria de las imágenes cargadas.
InstructionsScreen::~InstructionsScreen() {
    destroyAssets();
}

// Esta función genera 100 estrellas en posiciones aleatorias para el fondo.
void InstructionsScreen::generateStars() {
    stars.clear();
    const int starCount = 100;
    for (int i = 0; i < starCount; ++i) {
        stars.emplace_back(std::rand() % width, std::rand() % height);
    }
}

void InstructionsScreen::updateStars() {
    for (auto& star : stars) {
        star.second += 1.0f;  // velocidad constante hacia abajo

        if (star.second > height) {
            star.first = std::rand() % width;
            star.second = 0;  // reaparece arriba
        }
    }
}

// Esta función carga las imágenes de las teclas que se mostrarán en pantalla.
// Si alguna imagen no se encuentra, muestra un mensaje de error por consola.
void InstructionsScreen::loadAssets() {
    arrows_img = al_load_bitmap("assets/flechas.png");
    space_img = al_load_bitmap("assets/espacio.png");
    esc_img = al_load_bitmap("assets/esc.png");
    
    if (!arrows_img) std::cerr << "Error cargando flechas.png\n";
    if (!space_img) std::cerr << "Error cargando espacio.png\n";
    if (!esc_img) std::cerr << "Error cargando esc.png\n";
}

// Esta función libera la memoria de las imágenes si estaban cargadas.
void InstructionsScreen::destroyAssets() {
    if (arrows_img) al_destroy_bitmap(arrows_img);
    if (space_img) al_destroy_bitmap(space_img);
    if (esc_img) al_destroy_bitmap(esc_img);
}

// Esta función muestra la pantalla de instrucciones y espera a que el usuario presione ENTER o ESC.
// Si el usuario presiona ENTER, devuelve true (para empezar el juego). Si presiona ESC o cierra la ventana, devuelve false.
bool InstructionsScreen::run(SystemResources& sys) {
    al_start_timer(sys.timer);
    bool running = true;
    bool startGame = false;
    loadingTime = 0.0f;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(sys.eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } 
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            loadingTime += 1.0f / 120.0f;
            updateStars();
            if (loadingTime >= 2.0f) {
                showEnterText = true;
                // Efecto de pulsación suave (oscila entre 0.3 y 1.0 cada 2 segundos)
                pulseIntensity = 0.65f + 0.35f * sin(al_get_time() * 3.0f);
            }
            draw();
        } 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER && showEnterText) {
                startGame = true;
                running = false;
            } 
            else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
        }
    }
    return startGame;
}

// Esta función se encarga de dibujar toda la pantalla de instrucciones.
void InstructionsScreen::draw() const {
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

    // Constantes para posicionar los elementos en pantalla.
    const int TITLE_Y = 80;                   // Posición del título
    const int CONTROLS_Y = 250;               // Posición de los nombres de controles
    const int KEY_Y = CONTROLS_Y + 50;        // Posición de las imágenes de teclas
    const int KEY_IMG_HEIGHT = 90;            // Altura estándar para las imágenes
    //const int LOADING_Y = 520;              // Posición del mensaje de carga
    const int COL1_X = 200;                   // Columna 1 (moverse)
    const int COL2_X = 400;                   // Columna 2 (disparar)
    const int COL3_X = 600;                   // Columna 3 (salir)

    // Dibuja el título de la pantalla de instrucciones con un color azul claro.
    al_draw_text(font, al_map_rgb(100, 180, 255), COL2_X, TITLE_Y, ALLEGRO_ALIGN_CENTER, "INSTRUCTIONS");

    // Dibuja los nombres de los controles, cada uno con un color diferente para distinguirlos.
    al_draw_text(font, al_map_rgb(255, 255, 255), COL1_X, CONTROLS_Y, ALLEGRO_ALIGN_CENTER, "MOVE");
    al_draw_text(font, al_map_rgb(255, 200, 100), COL2_X, CONTROLS_Y, ALLEGRO_ALIGN_CENTER, "SHOOT");
    al_draw_text(font, al_map_rgb(255, 100, 100), COL3_X, CONTROLS_Y, ALLEGRO_ALIGN_CENTER, "EXIT");

    // Dibuja las imágenes de las teclas, escaladas para que todas tengan la misma altura.
    if (arrows_img) {
        int img_w = al_get_bitmap_width(arrows_img);
        int img_h = al_get_bitmap_height(arrows_img);
        float scale = (float)KEY_IMG_HEIGHT / img_h;
        al_draw_scaled_bitmap(arrows_img, 0, 0, img_w, img_h, COL1_X - (img_w*scale)/2, KEY_Y, img_w*scale, KEY_IMG_HEIGHT, 0);
    }

    if (space_img) {
        int img_w = al_get_bitmap_width(space_img);
        int img_h = al_get_bitmap_height(space_img);
        float scale = (float)KEY_IMG_HEIGHT / img_h;
        al_draw_scaled_bitmap(space_img, 0, 0, img_w, img_h, COL2_X - (img_w*scale)/2, KEY_Y, img_w*scale, KEY_IMG_HEIGHT, 0);
    }

    if (esc_img) {
        int img_w = al_get_bitmap_width(esc_img);
        int img_h = al_get_bitmap_height(esc_img);
        float scale = (float)KEY_IMG_HEIGHT / img_h;
        al_draw_scaled_bitmap(esc_img, 0, 0, img_w, img_h, COL3_X - (img_w*scale)/2, KEY_Y, img_w*scale, KEY_IMG_HEIGHT, 0);
    }

    const char* message = showEnterText ? 
        "PRESS ENTER TO PLAY" : 
        "LOADING...";
    
    if (showEnterText) {
        pulseIntensity = 0.05f + 0.95f * pow(sin(al_get_time() * 2.0f), 4.0f);  // PARPADEO
        
        // Verde puro que pulsa (sin mezcla de colores)
        ALLEGRO_COLOR pulseColor = al_map_rgb(
            0,                              // R
            30 + 225 * pulseIntensity,     // G
            0                               // B
        );
        
        // Texto con sombra negra
        al_draw_text(font, al_map_rgb(0, 0, 0), width/2 + 2, 522, ALLEGRO_ALIGN_CENTER, message);
        // Texto principal pulsante
        al_draw_text(font, pulseColor, width/2, 520, ALLEGRO_ALIGN_CENTER, message);
    } else {
        al_draw_text(font, al_map_rgb(200, 200, 200), width/2, 520, ALLEGRO_ALIGN_CENTER, message);
    }

    al_flip_display();
}