#include <SDL2/SDL.h>
#include "../include/mcga.h"

int main(int argc, char *argv[]) {
    init();
    uint8_t *vscreen = get_vscreen();
    uint16_t *maske = (uint16_t*) malloc(SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t));
    uint8_t *map = (uint8_t*) malloc(65536);

    memset(map, 0, 65536);
    for (uint i = 0; i < 256; i++) {
        set_pal(i, 15, 15, i);
    }

    plasma(0, 0, 256, 2, map);
    uint32_t off = 0;
    const double SCALE_FACTOR = 40.743666 * 2.0;
    const int SCREEN_WIDTH_HALF = SCREEN_WIDTH / 2;
    const int SCREEN_HEIGHT_HALF = SCREEN_HEIGHT / 2;
    const double DIST_SCALE = 700.0;
    const double ANGLE_SCALE = 1024.0;
    const double ASPECT_RATIO = 1.6;

    for (int y = -SCREEN_HEIGHT_HALF; y < SCREEN_HEIGHT_HALF; ++y) {
        for (int x = -SCREEN_WIDTH_HALF; x < SCREEN_WIDTH_HALF; ++x) {
            double distance = sqrt(static_cast<double>(x * x + y * y));
            double normalized_distance = sqrt(DIST_SCALE * distance) / 2.0;
            int c =
                    static_cast<int>(255.0 * (1.0 - normalized_distance / 255.0));
            double angle =
                    (y == 0) ?
                            atan(x * ANGLE_SCALE) :
                            atan(static_cast<double>(x) / (y * ASPECT_RATIO));
            int d = static_cast<int>(SCALE_FACTOR * angle);
            maske[off++] = ((d & 0xFF) << 8) | (c & 0xFF);
        }
    }

    while (1) {
        int t = SDL_GetTicks() / 20;
        uint32_t mov = (t << 8) + t;
        off = 0;
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

