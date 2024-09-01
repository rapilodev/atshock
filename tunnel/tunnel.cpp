#include <SDL2/SDL.h>
#include "../include/mcga.h"

int main(int argc, char *argv[]) {
    init();
    uint8_t *vscreen = get_vscreen();
    uint16_t *maske = (uint16_t*) malloc(
    SCREEN_WIDTH * SCREEN_WIDTH * sizeof(uint16_t));
    uint8_t *map = (uint8_t*) malloc(256 * 256);

    map[0] = 255;
    for (int i = 1; i < 256 * 256; i++) {
        map[i] = 0;
    }
    plasma(0, 0, 256, 2, map);

    for (int i = 0; i < 256; i++) {
        set_pal(i, 15, 15, i);
    }
    clear_screen();

    uint32_t off = 0;
    for (int j = -SCREEN_HEIGHT / 2; j < SCREEN_HEIGHT / 2; j++) {
        for (int i = -SCREEN_WIDTH / 2; i < SCREEN_WIDTH / 2; i++) {
            uint16_t c;
            if (j == 0)
                c = 40.743666 * atan(i << 10);
            else
                c = 40.743666 * atan(i / (j * 1.6));
            c = (c << 8) + 255
                    - (wurzel((200 * wurzel(((i * i) + (j * j)) | 1)) | 1))
                    >> 1;
            maske[off++] = c;
        }
    }

    while (1) {
        int t = SDL_GetTicks() / 50;
        uint32_t off = 0;
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                vscreen[off] = map[(maske[off] + t) & 65535];
                off++;
            }
        }
        handle_events (NULL);
        draw_screen();
    }
    shutdown();
}
