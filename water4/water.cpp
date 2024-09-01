#include <SDL2/SDL.h>
#include "../include/mcga.h"

// Adapted from 1997 Watcom C DOS sources to SDL2

void copy_circle(int xm, int ym, int r, uint8_t *source, uint8_t *target) {
    const uint maxr = 1;
    int y = r;
    int x = 0;
    do {
        if (r < 0) {
            y--;
            r += y + y;
        }
        for (int h = -1; h <= 1; h += 2) {
            for (int w = -1; w <= 1; w += 2) {
                int add = SCREEN_WIDTH * (ym + h * y) + xm + w * x;
                copy_pixel(source, target, add + h * SCREEN_WIDTH + w, add);
                add = SCREEN_WIDTH * (ym + h * x) + xm + w * y;
                copy_pixel(source, target, add + w * SCREEN_WIDTH + h, add);
            }
        }
        r -= x + x + 1;
        x++;
    } while (y >= x);
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

    const int n = 20;
    const int rmax = SCREEN_WIDTH;
    int x[n], y[n], r[n];
    for (int i = 0; i < n; i++) {
        x[i] = get_random(SCREEN_WIDTH);
        y[i] = get_random(SCREEN_HEIGHT);
        r[i] = get_random(rmax);
    }

    while (1) {
        copy_screen(img, vscreen);
        int t = SDL_GetTicks() / 10;

        for (int i = 0; i < n; i++) {
            if (r[i] < rmax)
                r[i]++;
            else {
                x[i] = get_random(SCREEN_WIDTH);
                y[i] = get_random(SCREEN_HEIGHT);
                r[i] = get_random(rmax);
            }
            copy_circle(x[i], y[i], r[i], vscreen, vscreen);
        }
        handle_events (NULL);
        draw_screen();
    }
    shutdown();
}
