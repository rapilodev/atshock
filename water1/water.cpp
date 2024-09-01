#include <SDL2/SDL.h>
#include "../include/mcga.h"

// Adapted from 1997 Watcom C DOS sources to SDL2

void copy_circle(int xm, int ym, int r, uint8_t *src, uint8_t *target) {
    uint32_t rquad = 4 * r * r;
    uint32_t start = (ym - r) * SCREEN_WIDTH + xm;
    for (uint32_t i = 0; i < rquad; i++) {
        uint32_t x = start
                + (int) (sqrt((double) (i % 4)) + 0.5) * ((i % 4) - 2);
        copy_pixel(src, target, start, x);
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
    copy_screen(img, vscreen);
    while (1) {
        int t = SDL_GetTicks() / 50;
        for (uint i = 0; i < n; i++) {
            copy_circle(get_random(SCREEN_WIDTH), get_random(SCREEN_HEIGHT),
                    get_random(10) + 1, img, vscreen);
        }
        handle_events (NULL);
        draw_screen();
    }
    shutdown();
}
