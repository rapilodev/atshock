#include <SDL2/SDL.h>
#include "../include/mcga.h"

int main(int argc, char *argv[]) {
    init();
    uint8_t *vscreen = get_vscreen();
    uint16_t *maske = (uint16_t*) malloc(
    SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));
    uint8_t *map = (uint8_t*) malloc(65536);
    memset(map, 0, 65536);
    for (uint i = 0; i < 256; i++) {
        set_pal(i, 15, 15, i);
    }

    int off = 0;
    double scale = 40.743666 * 2.0;
    for (int j = -SCREEN_HEIGHT / 2; j < SCREEN_HEIGHT / 2; j++) {
        for (int i = -SCREEN_WIDTH / 2; i < SCREEN_WIDTH / 2; i++) {
            long int dist = wurzel(i * i + j * j);
            int c = 255 - (wurzel(700 * dist)) / 2;
            int d = (j == 0) ? scale * atan(i * 1024.0) : scale * atan(i / (j * 1.6));
            maske[off++] = ((d << 8) + c) & 65535;
        }
    }

    while (1) {
        int t = SDL_GetTicks() / 20;
        for (int i = 0; i < 65536; i += get_random(8)) {
            map[i] = 0;
        }
        plasma(0, 0, 256, 2, map);

        uint32_t mov = (t << 8) + t;
        uint32_t off = 0;
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                vscreen[off] = map[(maske[off] + mov) & 65535];
                off++;
            }
        }
        handle_events (NULL);
        draw_screen();
    }
    shutdown();

}

