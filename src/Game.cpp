#include "Game.hpp"
#include <allegro5/allegro_image.h>
#include <cstdlib>
#include <ctime>
#include <allegro5/color.h>

#include <iostream>


struct Star {
    float x, y;
    float brightness;
};

const int NUM_STARS = 100;
Star stars[NUM_STARS];

void initStars(int width, int height) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].x = std::rand() % width;
        stars[i].y = std::rand() % height;
        stars[i].brightness = 0.5f + static_cast<float>(std::rand()) / RAND_MAX * 0.5f;
    }
}

void drawStars() {
    for (const auto& star : stars) {
        ALLEGRO_COLOR color = al_map_rgb_f(star.brightness, star.brightness, star.brightness);
        al_draw_pixel(star.x, star.y, color);
    }
}

void runGame(SystemResources& sysRes) {
    al_init_image_addon();

    ALLEGRO_BITMAP* player = al_load_bitmap("assets/player.png");
    if (!player) {
        std::cerr << "Failed to load player image.\n";
        return;
    }

    const float playerW = al_get_bitmap_width(player);
    const float playerH = al_get_bitmap_height(player);
    float playerX = (sysRes.width - playerW) / 2.0f;
    float playerY = sysRes.height - playerH - 10;

    initStars(sysRes.width, sysRes.height);

    bool running = true;
    bool redraw = true;
    al_start_timer(sysRes.timer);

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(sysRes.eventQueue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                running = false;
        }

        if (redraw && al_is_event_queue_empty(sysRes.eventQueue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            drawStars();
            al_draw_bitmap(player, playerX, playerY, 0);
            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_bitmap(player);
}
