#include <SDL2/SDL.h>
#include "../include/mcga.h"

// Adapted from 1997 Watcom C DOS sources to SDL2

void copy_circle(int xm, int ym, int r, uint8_t *src, uint8_t *target) {
    int r_squared = r * r;
    int dx = get_random(r) - r / 2;
    int dy = get_random(r) - r / 2;
    for (int y = -r; y <= r; ++y) {
        for (int x = -r; x <= r; ++x) {
            if (x * x + y * y <= r_squared) {
                int src_x = xm + x;
                int src_y = ym + y;
                int src_x_pos = src_x + dx;
                int src_y_pos = src_y + dy;
                int target_index = src_y * SCREEN_WIDTH + src_x;
                if (src_x
                        >= 0&& src_x < SCREEN_WIDTH && src_y >= 0 && src_y < SCREEN_HEIGHT) {
                    int src_pos = src_y_pos * SCREEN_WIDTH + src_x_pos;
                    copy_pixel(src, target, src_pos, target_index);
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    const char *image_path = "../images/stones-water-256.pcx";
    if (argc > 1)
        image_path = argv[1];
    init();
    uint8_t *vscreen = get_vscreen();
    uint8_t *img = (uint8_t*) malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
    uint8_t img_pal[3 * 256];
    read_pcx(image_path, img, img_pal);
    set_fullpal(img_pal);

    const uint n = 1000;
    const uint maxr = 6;
    while (1) {
        copy_screen(img, vscreen);
        int t = SDL_GetTicks() / 50;
        for (uint i = 0; i < n; i++) {
            copy_circle(get_random(SCREEN_WIDTH), get_random(SCREEN_HEIGHT),
                    get_random(maxr) + 1, img, vscreen);
        }
        handle_events (NULL);
        draw_screen();
    }
    shutdown();
}
