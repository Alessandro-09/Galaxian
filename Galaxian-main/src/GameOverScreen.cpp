#include "GameOverScreen.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <iostream>

GameOverScreen::GameOverScreen(ALLEGRO_FONT* font, int width, int height)
  : font(font)
  , screenWidth(width)
  , screenHeight(height)
  , sample(nullptr)
  , instance(nullptr)
{}

std::string GameOverScreen::show(SystemResources& sys, int score) {
    // Texto de score
    std::string scoreText = "Score: " + std::to_string(score);

    // Buffer para el nombre de 3 letras
    char nameInput[4] = { '\0' };
    int charIndex = 0;
    bool entering = true;

    // Carga y reproduce la música (una sola vez)
    sample = al_load_sample("assets/gameover.ogg");
    if (sample) {
        instance = al_create_sample_instance(sample);
        al_attach_sample_instance_to_mixer(instance, al_get_default_mixer());
        al_set_sample_instance_playmode(instance, ALLEGRO_PLAYMODE_ONCE);
        al_set_sample_instance_gain(instance, 1.0f);
        al_play_sample_instance(instance);
    }

    // Bucle de pantalla + captura de teclas
    while (entering) {
        al_clear_to_color(al_map_rgb(0,0,0));

        al_draw_text(font, al_map_rgb(255,0,0), screenWidth/2, screenHeight/4, ALLEGRO_ALIGN_CENTER, "GAME OVER");
        al_draw_text(font, al_map_rgb(255,255,255), screenWidth/2, screenHeight/4+60, ALLEGRO_ALIGN_CENTER, scoreText.c_str());
        al_draw_text(font, al_map_rgb(180,180,180), screenWidth/2, screenHeight/2-20, ALLEGRO_ALIGN_CENTER, "Escribe tu nombre (3 letras):");
        al_draw_text(font, al_map_rgb(255,255,0), screenWidth/2, screenHeight/2+30, ALLEGRO_ALIGN_CENTER, nameInput);

        if (charIndex < 3) {
            al_draw_text(font, al_map_rgb(150,150,150), screenWidth/2, screenHeight-60, ALLEGRO_ALIGN_CENTER, "Escribe letras...");
        } else {
            al_draw_text(font, al_map_rgb(100,255,100), screenWidth/2, screenHeight-60, ALLEGRO_ALIGN_CENTER, "ENTER para confirmar");
        }

        al_flip_display();

        ALLEGRO_EVENT ev;
        al_wait_for_event(sys.eventQueue, &ev);

        if (ev.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (charIndex < 3 && isalpha(ev.keyboard.unichar)) {
                nameInput[charIndex++] = toupper(ev.keyboard.unichar);
            } else if (ev.keyboard.unichar == '\b' && charIndex > 0) {
                nameInput[--charIndex] = '\0';
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN &&
                 ev.keyboard.keycode == ALLEGRO_KEY_ENTER &&
                 charIndex == 3) {
            entering = false;
        }
    }

    // Guardar y limpiar audio
    std::string result(nameInput);
    if (instance) al_destroy_sample_instance(instance);
    if (sample)   al_destroy_sample(sample);
    return result;
}sss