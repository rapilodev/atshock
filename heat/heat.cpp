#include <SDL2/SDL.h>
#include "../include/mcga.h"

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
    clear_screen();
    while (1) {
        int t = SDL_GetTicks() / 50;
        int off = SCREEN_WIDTH;
        for (int i = SCREEN_WIDTH; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            copy_pixel(img, vscreen, off,
                    off + get_random_int(1) + get_random_int(1) * SCREEN_WIDTH);
            off++;
        }
        handle_events (NULL);
        draw_screen();
    }
    shutdown();

}

