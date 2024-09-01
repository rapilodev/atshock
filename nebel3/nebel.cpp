#include <SDL2/SDL.h>
#include "../include/mcga.h"

int main(int argc, char *argv[]) {
    init();
    uint8_t *vscreen = get_vscreen();
    uint16_t *maske = (uint16_t*) malloc(
            SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));
    uint8_t *map = (uint8_t*) malloc(65536);
    uint8_t *map2 = (uint8_t*) malloc(65536);

    unsigned off = 0;
    for (int i = -SCREEN_HEIGHT / 2; i < 0; i++)
        for (int j = -SCREEN_WIDTH / 2; j < SCREEN_WIDTH / 2; j++) {
            maske[off] = (255
                    - ((SCREEN_WIDTH / SCREEN_HEIGHT
                            / (wurzel(((i * i) + (j * j)) | 1) | 1)) >> 2))
                    << 8;
            if (j == 0)
                vscreen[off] = 81.48733086 * atan(i << 10);
            else
                vscreen[off] = 81.48733086 * atan((i * 1.6) / j);
            off++;
        }
    off = 31680;
    for (int i = 0; i < SCREEN_WIDTH / 2; i++)
        vscreen[off++] = 0;
    for (uint i = 0; i < SCREEN_WIDTH / 2; i++)
        vscreen[off++] = 255;
    for (uint i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT/2; i++)
        maske[i + SCREEN_WIDTH * SCREEN_HEIGHT / 2] = maske[SCREEN_WIDTH
                * SCREEN_HEIGHT / 2 - 1 - i]
                + ((vscreen[SCREEN_WIDTH * SCREEN_HEIGHT / 2 - 1 - i] >> 1) + 128);
    for (uint i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT/2; i++)
        maske[i] += (vscreen[i] >> 1);

    plasma(0, 0, 256, 4, map);
    plasma(0, 0, 256, 4, map2);
    for (uint i = 0; i < 65536; i++) {
        map[i] = map[i] >> 1;
        map2[i] = map2[i] >> 1;
    }

    for (uint i = 0; i < 128; i++)
        set_pal(i, 0, 0, i >> 1);

    for (uint i = 0; i < 128; i++)
        set_pal(i + 128, i >> 1, i >> 1, 63);

    while (1) {
        int t = SDL_GetTicks() / 20;

        uint32_t mov = (t << 8) + t;
        off = t & 1;
        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT / 2; i++) {
            int c = maske[off];
            vscreen[off] = map[(c + mov) & 65535] + map2[(c - mov) & 65535];
            off += 2;
        }

        handle_events (NULL);
        draw_screen();
    }
    shutdown();

}

