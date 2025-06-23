#include "HighScore.hpp"
#include <allegro5/allegro_primitives.h>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <sstream>

// CONFIGURACIÓN GLOBAL
// Nombre del archivo donde se guardan las puntuaciones altas
const std::string HighScore::SCORES_FILE = "highscores.txt";

// CONSTRUCTOR Y DESTRUCTOR

// Constructor: Inicializa el sistema de puntuaciones altas
HighScore::HighScore(ALLEGRO_FONT* font, int width, int height)
    : font(font), width(width), height(height) {
    generateStars();    // Crear estrellas para el fondo animado
    loadScores();       // Cargar puntuaciones guardadas desde archivo
}

// Destructor: Limpia recursos (actualmente no hay recursos específicos que liberar)
HighScore::~HighScore() {}

//  MANEJO DE EFECTOS VISUALES

// Genera estrellas aleatorias para el fondo animado de la pantalla
void HighScore::generateStars() {
    stars.clear();      // Limpiar estrellas existentes
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Semilla aleatoria basada en tiempo
    const int starCount = 100;   // Número total de estrellas en el fondo
    
    for (int i = 0; i < starCount; ++i) {
        // Crear estrella con posición aleatoria (x, y)
        stars.emplace_back(std::rand() % width, std::rand() % height);
    }
}

// Actualiza la posición de las estrellas para crear efecto de movimiento
void HighScore::updateStars() {
    for (auto& star : stars) {
        star.second += 1.0f;    // Mover estrella hacia abajo
        
        // Si la estrella sale por la parte inferior, reaparecerla arriba
        if (star.second > height) {
            star.first = std::rand() % width;  // Nueva posición X aleatoria
            star.second = 0;                   // Resetear posición Y al tope
        }
    }
}

//  MANEJO DE PUNTUACIONES

// Agrega una nueva puntuación a la lista y la mantiene ordenada
void HighScore::addScore(int score, const std::string& name) {
    // Agregar la nueva entrada a la lista
    scores.emplace_back(score, name);

    // Ordenar todas las puntuaciones de mayor a menor
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;  // Comparación: puntuación más alta primero
    });

    // Mantener solo las 10 mejores puntuaciones
    if (scores.size() > 10) {
        scores.resize(10);  // Recortar la lista a las 10 mejores
    }

    saveScores();  // Guardar la lista actualizada en el archivo
}

// Carga las puntuaciones guardadas desde el archivo
void HighScore::loadScores() {
    scores.clear();  // Limpiar puntuaciones existentes en memoria
    std::ifstream file(SCORES_FILE);  // Abrir archivo para lectura

    if (file.is_open()) {
        std::string line;
        
        // Leer el archivo línea por línea
        while (std::getline(file, line)) {
            std::istringstream iss(line);  // Stream para procesar cada línea
            std::string name;
            int score;
            
            // Extraer nombre y puntuación de cada línea
            if (iss >> name >> score) {
                scores.emplace_back(score, name);  // Agregar entrada a la lista
            }
        }
        file.close();
    }

    // Ordenar las puntuaciones cargadas de mayor a menor
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });
}

// Guarda todas las puntuaciones actuales en el archivo
void HighScore::saveScores() {
    std::ofstream file(SCORES_FILE);  // Abrir archivo para escritura

    if (file.is_open()) {
        // Escribir cada entrada en el formato: "nombre puntuación"
        for (const auto& entry : scores) {
            file << entry.name << " " << entry.score << std::endl;
        }
        file.close();
    }
}

// Devuelve las mejores puntuaciones hasta el número especificado
std::vector<ScoreEntry> HighScore::getTopScores(int count) {
    std::vector<ScoreEntry> topScores;  // Vector para las mejores puntuaciones
    
    // Determinar cuántas puntuaciones devolver (mínimo entre count y puntuaciones disponibles)
    int limit = std::min(count, static_cast<int>(scores.size()));

    // Copiar las mejores puntuaciones al vector resultado
    for (int i = 0; i < limit; ++i) {
        topScores.push_back(scores[i]);
    }

    return topScores;
}


// Ejecuta la pantalla de puntuaciones altas (bucle principal)
void HighScore::run(SystemResources& sys) {
    al_start_timer(sys.timer);  // Iniciar cronómetro para animaciones
    bool running = true;

    // ===== MANEJO DE MÚSICA =====
    // Asegurar que la música del menú esté reproduciéndose
    if (sys.menuMusic && !al_get_audio_stream_playing(sys.menuMusic)) {
        al_set_audio_stream_playing(sys.menuMusic, true);
    }

    // Detener música de instrucciones si estaba reproduciéndose
    if (sys.instructionsMusic && al_get_audio_stream_playing(sys.instructionsMusic)) {
        al_set_audio_stream_playing(sys.instructionsMusic, false);
    }

    //BUCLE PRINCIPA
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(sys.eventQueue, &event);  // Esperar por eventos

        if (event.type == ALLEGRO_EVENT_TIMER) {
            // Actualizar y dibujar en cada tick del cronómetro
            updateStars();  // Mover las estrellas del fondo
            draw();         // Dibujar toda la pantalla
        } 
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Usuario cerró la ventana
            running = false;
        } 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Manejar teclas para salir de la pantalla
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE ||
                event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                running = false;  // Salir con ESC o ENTER
            }
        }
    }
}

//RENDERIZADO DE LA PANTALLA 

// Dibuja toda la pantalla de puntuaciones altas
void HighScore::draw() const {
    // Limpiar pantalla con fondo azul oscuro espacial
    al_clear_to_color(al_map_rgb(3, 5, 15));
    
    // Obtener tiempo actual para efectos de animación
    float t = al_get_time();

    // DIBUJAR ESTRELLAS DEL FONDO
    for (const auto& star : stars) {
        // Calcular efecto de parpadeo basado en tiempo y posición
        float twinkle = 0.7f + 0.3f * std::sin(t * 3 + star.first * 10);
        int val = 150 + static_cast<int>(105 * twinkle);  // Intensidad de brillo
        
        // Dibujar estrella con un ligero tinte azulado
        al_draw_pixel(star.first, star.second, al_map_rgb(val, val, val + 30));
    }

    //TÍTULO PRINCIPAL
    al_draw_text(font, al_map_rgb(100, 180, 255), width / 2, 50,
                 ALLEGRO_ALIGN_CENTER, "HIGH SCORES");

    //  TABLA DE PUNTUACIONES 
    // Obtener las 5 mejores puntuaciones para mostrar
    auto topScores = const_cast<HighScore*>(this)->getTopScores(5);
    int startY = 150;      // Posición Y donde empieza la tabla
    int lineHeight = 60;   // Espacio vertical entre cada entrada

    // Dibujar cada entrada de puntuación
    for (size_t i = 0; i < topScores.size(); ++i) {
        // Preparar textos para cada columna
        std::string rank = std::to_string(i + 1) + ".";        // Ranking (1., 2., 3., etc.)
        std::string name = topScores[i].name;                   // Nombre del jugador
        std::string scoreText = std::to_string(topScores[i].score); // Puntuación

        int y = startY + i * lineHeight;  // Calcular posición Y de esta entrada

        // Dibujar cada columna con diferentes colores
        al_draw_text(font, al_map_rgb(255, 255, 255), 100, y, ALLEGRO_ALIGN_LEFT, rank.c_str());      // Ranking en blanco
        al_draw_text(font, al_map_rgb(255, 100, 255), 160, y, ALLEGRO_ALIGN_LEFT, name.c_str());      // Nombre en magenta
        al_draw_text(font, al_map_rgb(255, 255, 0), 400, y, ALLEGRO_ALIGN_LEFT, scoreText.c_str());   // Puntuación en amarillo
    }

    // INSTRUCCIONES AL USUARIO 
    al_draw_text(font, al_map_rgb(100, 150, 200), width / 2, height - 50,
                 ALLEGRO_ALIGN_CENTER, "Presiona ENTER o ESC para volver");

    al_flip_display();  // Mostrar todo en pantalla
}