#include "HighScore.hpp"
#include <allegro5/allegro_primitives.h>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <iostream>
#include <cstdlib>  // Para srand() y rand()

const std::string HighScore::SCORES_FILE = "highscores.txt";

HighScore::HighScore(ALLEGRO_FONT* font, int width, int height)
    : font(font), width(width), height(height) {
    generateStars();
    loadScores();
}

HighScore::~HighScore() {}

void HighScore::generateStars() {
    stars.clear();
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    const int starCount = 100;
    for (int i = 0; i < starCount; ++i) {
        stars.emplace_back(std::rand() % width, std::rand() % height);
    }
}

void HighScore::updateStars() {
    for (auto& star : stars) {
        star.second += 1.0f;
        if (star.second > height) {
            star.first = std::rand() % width;
            star.second = 0;
        }
    }
}

std::string HighScore::getCurrentDate() {
    std::time_t now = std::time(nullptr);
    char* dt = std::ctime(&now);
    std::string date(dt);
    // Remover el \n del final
    if (!date.empty() && date[date.length()-1] == '\n') {
        date.erase(date.length()-1);
    }
    return date;
}

void HighScore::addScore(int score) {
    scores.emplace_back(score, getCurrentDate());
    
    // Ordenar por puntaje descendente
    std::sort(scores.begin(), scores.end(), 
              [](const ScoreEntry& a, const ScoreEntry& b) {
                  return a.score > b.score;
              });
    
    // Mantener solo los top 10
    if (scores.size() > 10) {
        scores.resize(10);
    }
    
    saveScores();
}

void HighScore::loadScores() {
    scores.clear();
    std::ifstream file(SCORES_FILE);
    
    if (file.is_open()) {
        int score;
        std::string date;
        
        while (file >> score) {
            file.ignore(); // Ignorar el espacio
            std::getline(file, date);
            scores.emplace_back(score, date);
        }
        file.close();
    }
    
    // Ordenar por puntaje descendente
    std::sort(scores.begin(), scores.end(), 
              [](const ScoreEntry& a, const ScoreEntry& b) {
                  return a.score > b.score;
              });
}

void HighScore::saveScores() {
    std::ofstream file(SCORES_FILE);
    
    if (file.is_open()) {
        for (const auto& entry : scores) {
            file << entry.score << " " << entry.date << std::endl;
        }
        file.close();
    }
}

std::vector<ScoreEntry> HighScore::getTopScores(int count) {
    std::vector<ScoreEntry> topScores;
    int limit = std::min(count, static_cast<int>(scores.size()));
    
    for (int i = 0; i < limit; ++i) {
        topScores.push_back(scores[i]);
    }
    
    return topScores;
}

void HighScore::run(SystemResources& sys) {
    al_start_timer(sys.timer);
    bool running = true;
    
    // Música del menú
    if (sys.menuMusic && !al_get_audio_stream_playing(sys.menuMusic)) {
        al_set_audio_stream_playing(sys.menuMusic, true);
    }
    
    if (sys.instructionsMusic && al_get_audio_stream_playing(sys.instructionsMusic)) {
        al_set_audio_stream_playing(sys.instructionsMusic, false);
    }
    
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(sys.eventQueue, &event);
        
        if (event.type == ALLEGRO_EVENT_TIMER) {
            updateStars();
            draw();
        } 
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } 
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE || 
                event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                running = false;
            }
        }
    }
}

void HighScore::draw() const {
    // Fondo azul oscuro
    al_clear_to_color(al_map_rgb(3, 5, 15));
    
    float t = al_get_time();
    
    // Dibujar estrellas
    for (const auto& star : stars) {
        float twinkle = 0.7f + 0.3f * std::sin(t * 3 + star.first * 10);
        if (static_cast<int>(star.first + star.second) % 3 == 0) {
            int blue = 150 + static_cast<int>(105 * twinkle);
            al_draw_pixel(star.first, star.second, al_map_rgb(50, 100, blue));
        } else {
            int val = 150 + static_cast<int>(105 * twinkle);
            al_draw_pixel(star.first, star.second, al_map_rgb(val, val, val + 30));
        }
    }
    
    // Título
    al_draw_text(font, al_map_rgb(100, 180, 255), width/2, 50, 
                 ALLEGRO_ALIGN_CENTER, "HIGH SCORES");
    
    // Obtener top 5 puntajes
    auto topScores = const_cast<HighScore*>(this)->getTopScores(5);
    
    // Dibujar puntajes
    int startY = 150;
    int lineHeight = 60;
    
    for (size_t i = 0; i < topScores.size(); ++i) {
        std::string rank = std::to_string(i + 1) + ".";
        std::string scoreText = std::to_string(topScores[i].score);
        
        // Número de ranking
        al_draw_text(font, al_map_rgb(255, 255, 255), 150, startY + i * lineHeight, 
                     ALLEGRO_ALIGN_LEFT, rank.c_str());
        
        // Puntaje
        al_draw_text(font, al_map_rgb(255, 255, 0), 300, startY + i * lineHeight, 
                     ALLEGRO_ALIGN_LEFT, scoreText.c_str());
    }
    
    // Si no hay puntajes
    if (topScores.empty()) {
        al_draw_text(font, al_map_rgb(150, 150, 150), width/2, 200, 
                     ALLEGRO_ALIGN_CENTER, "No scores yet!");
    }
    
    // Instrucciones
    al_draw_text(font, al_map_rgb(100, 150, 200), width/2, height - 50, 
                 ALLEGRO_ALIGN_CENTER, "Press ESC or ENTER to return");
    
    al_flip_display();
}