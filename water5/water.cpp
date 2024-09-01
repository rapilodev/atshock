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

    const int n = 50;
    const int rmax = SCREEN_WIDTH / 2;
    int x[n], y[n], r[n];
    for (int i = 0; i < n; i++) {
        x[i] = get_random(SCREEN_WIDTH);
        y[i] = get_random(SCREEN_HEIGHT);
        r[i] = get_random(rmax);
    }

    int sines[256];
    for (int i = 0; i < 256; i++)
        sines[i] = sin(3.141592654 * i / 128) * 127;
    int coses[256];
    for (int i = 0; i < 256; i++)
        coses[i] = cos(3.141592654 * i / 128) * 127;

    while (1) {
        int t = SDL_GetTicks() / 2;
        copy_screen(img, vscreen);
        int c = t * 2;

        for (int i = 0; i < n; i++) {
            if (r[i] < rmax)
                r[i]++;
            else {
                x[i] = SCREEN_WIDTH / 2 + (sines[c & 255] >> 2)
                        + (sines[t & 255] >> 2);
                y[i] = SCREEN_HEIGHT / 2 + (coses[c & 255] >> 2)
                        + (coses[t & 255] >> 2);
                r[i] = 1;
            }
            copy_circle(x[i], y[i], r[i], vscreen, vscreen);
        }
        handle_events (NULL);
        draw_screen();
    }
    shutdown();
}
