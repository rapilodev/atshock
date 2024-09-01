#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ostream>
#include "mcga.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
uint8_t *vscreen = NULL;
SDL_Color *palette = NULL;
uint32_t prev_ticks = 0;

uint8_t* get_vscreen() {
    return vscreen;
}
SDL_Color* get_palette() {
    return palette;
}
SDL_Renderer* get_renderer() {
    return renderer;
}
SDL_Texture* get_texture() {
    return texture;
}
SDL_Window* get_window() {
    return window;
}

void mousemove(int *x, int *y) {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    *x = mouseX;
    *y = mouseY;
}

void init_random(void) {
    srand (time(NULL));}

int get_random(uint max) {
    return rand() % max;
}

int get_random_int(uint max) {
    if (max==0)return 0;
    int v = 0;
    while (v == 0) {
        v = (rand() % (2 * max + 1)) - max;
    }
    return v;
}

void softscreen(uint8_t *screen) {
    for (int i = SCREEN_WIDTH; i < SCREEN_WIDTH * SCREEN_HEIGHT - SCREEN_WIDTH;
            i++) {
        screen[i] = (uint8_t)(
                (screen[i - 1] + screen[i + 1] + screen[i - SCREEN_WIDTH]
                        + screen[i + SCREEN_WIDTH]) >> 2);
    }
}

void softflip(uint8_t *src, uint8_t *dest) {
    for (int i = SCREEN_WIDTH; i < SCREEN_WIDTH * SCREEN_HEIGHT - SCREEN_WIDTH;
            i++) {
        dest[i] = (uint8_t)(
                (dest[i - 1] + src[i + 1] + dest[i - SCREEN_WIDTH]
                        + src[i + SCREEN_WIDTH]) >> 2);
    }
}

void put_pixel(uint x, uint y, uint8_t color, uint8_t *screen) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        screen[y * SCREEN_WIDTH + x] = color;
    }
}

void copy_pixel(uint8_t *source, uint8_t *target, uint32_t sadd,
        uint32_t tadd) {
    if (sadd
            < 0|| sadd >= SCREEN_WIDTH*SCREEN_HEIGHT || tadd < 0 || tadd >= SCREEN_WIDTH*SCREEN_HEIGHT
            )
        return;
    target[tadd] = source[sadd];
}

void line(int x1, int y1, int x2, int y2, uint8_t col, uint8_t *screen) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        put_pixel(x1, y1, col, screen);
        if (x1 == x2 && y1 == y2)
            break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void circle(int xm, int ym, int r, int col, uint8_t *screen) {
    int x = -r, y = 0, err = 2 - 2 * r;
    do {
        put_pixel(xm - x, ym + y, col, screen);
        put_pixel(xm - y, ym - x, col, screen);
        put_pixel(xm + x, ym - y, col, screen);
        put_pixel(xm + y, ym + x, col, screen);
        r = err;
        if (r <= y)
            err += ++y * 2 + 1;
        if (r > x || err > y)
            err += ++x * 2 + 1;
    } while (x < 0);
}

void rect(int x1, int y1, int x2, int y2, int c, uint8_t *screen) {
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            put_pixel(x, y, c, screen);
        }
    }
}

void polrect(int x1, int y1, int x2, int y2, int c1, int c2, uint8_t *screen) {
    int ipolx[SCREEN_WIDTH];
    int cpos = c1, xpos = x1;
    int cunit, len;
    int cdiff = c2 - c1;
    if (cdiff < 0) {
        cdiff = -cdiff;
        cunit = -1;
    } else
        cunit = 1;
    int xdiff = x2 - x1;
    int xerror = 0, cerror = 0;
    if (cdiff > xdiff)
        len = cdiff;
    else
        len = xdiff;
    int i = 0;
    while (i <= len) {
        cerror += cdiff;
        if (cerror > len) {
            cerror -= len;
            cpos += cunit;
        }
        xerror += xdiff;
        if (xerror > len) {
            xerror -= len;
            xpos++;
            ipolx[xpos] = cpos;
        }
        i++;
    }
    int off = y1 * SCREEN_WIDTH + x1;
    int xd = SCREEN_WIDTH - 1 - x2 + x1;
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++)
            screen[off++] = (char) ipolx[x];
        off += xd;
    }
}

//----------------------------------------------------------------------------------------------
void polrect2(int x1, int y1, int x2, int y2, int c1, int c2, int d1, int d2,
        uint8_t *screen) {
    int ipolx[SCREEN_WIDTH];
    int cpos = c1, xpos = x1, dpos = d1, ypos = y1;
    int cunit, dunit, xlen, ylen;
    int ydiff = y2 - y1;
    int ddiff = d2 - d1;
    if (ddiff < 0) {
        ddiff = -ddiff;
        dunit = -1;
    } else
        dunit = 1;
    if (ydiff > ddiff)
        ylen = ydiff;
    else
        ylen = ddiff;
    int xdiff = x2 - x1 + 1;
    int cdiff = c2 - c1;
    if (cdiff < 0) {
        cdiff = -cdiff;
        cunit = -1;
    } else
        cunit = 1;
    if (xdiff > cdiff)
        xlen = xdiff;
    else
        xlen = cdiff;
    int cerror = 0, xerror = 0, yerror = 0, derror = 0;
    int off = y1 * SCREEN_WIDTH + x1;
    int xd = SCREEN_WIDTH - 1 - x2 + x1;

    int i = 0;
    while (i <= xlen) {
        cerror += cdiff;
        if (cerror > xlen) {
            cerror -= xlen;
            cpos += cunit;
        }
        xerror += xdiff;
        if (xerror > xlen) {
            xerror -= xlen;
            xpos++;
            ipolx[xpos] = cpos;
        }
        i++;
    }

    i = 0;
    while (i <= ylen) {
        derror += ddiff;
        if (derror > ylen) {
            derror -= ylen;
            dpos += dunit;
        }
        yerror += ydiff;
        if (yerror > ylen) {
            yerror -= ylen;
            ypos++;
            for (int x = x1; x <= x2; x++)
                screen[off++] = (char) ((ipolx[x] + dpos) >> 1);
            off += xd;
        }
        i++;
    }
}

//----------------------------------------------------------------------------------------------
long int wurzel(long int a) {
    if (a == 0) return 0;
    long int x = a;
    long int y = (x + 1) >> 1;
    while (y < x) {
        x = y;
        y = (x + a / x) >> 1;
    }
    return x;
}

void plasma(int x1, int y1, int len, int rough, uint8_t *screen) {
    if (len <= 1) return;

    int x2 = (x1 + len) & 255;
    int y2 = (y1 + len) & 255;
    int lh = len >> 1;
    int xh = (x1 + lh) & 255;
    int yh = (y1 + lh) & 255;

    // Calculate the average of the corners
    int c = ((screen[x1 + (y1 << 8)] + screen[x2 + (y1 << 8)]
            + screen[x1 + (y2 << 8)] + screen[x2 + (y2 << 8)]) >> 2);

    // Set midpoints if not already set
    if (!screen[xh + (y1 << 8)]) screen[xh + (y1 << 8)] = c + get_random_int(rough);
    if (!screen[x1 + (yh << 8)]) screen[x1 + (yh << 8)] = c + get_random_int(rough);
    if (!screen[x2 + (yh << 8)]) screen[x2 + (yh << 8)] = c + get_random_int(rough);
    if (!screen[xh + (y2 << 8)]) screen[xh + (y2 << 8)] = c + get_random_int(rough);
    if (!screen[xh + (yh << 8)]) screen[xh + (yh << 8)] = c + get_random_int(rough);

    // Recursively apply to four quadrants
    rough >>= 1;
    plasma(x1, y1, lh, rough, screen);
    plasma(xh, y1, lh, rough, screen);
    plasma(x1, yh, lh, rough, screen);
    plasma(xh, yh, lh, rough, screen);
}

void fill_circle(int xm, int ym, int r, int col, uint8_t *screen) {
    int x = -r, y = 0, err = 2 - 2 * r;
    do {
        line(xm + x, ym - y, xm + x, ym + y, col, screen);
        line(xm - x, ym - y, xm - x, ym + y, col, screen);
        r = err;
        if (r <= y)
            err += ++y * 2 + 1;
        if (r > x || err > y)
            err += ++x * 2 + 1;
    } while (x < 0);
}

#define BSIZE 128

void read_pcx(const char *pcxname, unsigned char *screen, unsigned char *pal) {
    int fHandle = open(pcxname, O_RDONLY | O_BINARY); // O_BINARY is not needed on Linux
    if (fHandle < 0) {
        perror("open");
        return;
    }

    unsigned short x1, x2, y1, y2;
    lseek(fHandle, 4, SEEK_SET);
    read(fHandle, &x1, sizeof(x1));
    read(fHandle, &y1, sizeof(y1));
    read(fHandle, &x2, sizeof(x2));
    read(fHandle, &y2, sizeof(y2));
    x1 = x2 - x1 + 1;
    y1 = y2 - y1 + 1;

    int xress = x1;
    int yress = y1;
    uint32_t imgsize = xress * yress;

    // Move to the palette position
    lseek(fHandle, -768, SEEK_END);
    read(fHandle, pal, 768);

    lseek(fHandle, 128, SEEK_SET);

    unsigned char buff[BSIZE];
    int buffoff = BSIZE;
    unsigned char pb;
    uint32_t off = 0;

    while (off < imgsize) {
        if (buffoff == BSIZE) {
            buffoff = 0;
            ssize_t bytesRead = read(fHandle, buff, BSIZE);
            if (bytesRead <= 0) {
                perror("read");
                close(fHandle);
                return;
            }
        }
        pb = buff[buffoff++];
        if ((pb & 192) == 192) {
            int length = pb & 63;
            if (buffoff == BSIZE) {
                buffoff = 0;
                ssize_t bytesRead = read(fHandle, buff, BSIZE);
                if (bytesRead <= 0) {
                    perror("read");
                    close(fHandle);
                    return;
                }
            }
            pb = buff[buffoff++];
            for (int i = 0; i < length; i++) {
                screen[off++] = pb;
            }
        } else {
            screen[off++] = pb;
        }
    }

    close(fHandle);
}

void copy_screen(uint8_t * source, uint8_t * target) {
    memcpy(target, source, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint8_t));
}


void clear_screen() {
    memset(vscreen, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint8_t));
}

void set_pal(int n, uint8_t r, uint8_t g, uint8_t b) {
    palette[n].r = r;
    palette[n].g = g;
    palette[n].b = b;
}

void set_fullpal(uint8_t *pal) {
    for (int i = 0; i < 256; i++) {
        set_pal(i, pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]);
    }
}

void init() {
    init_random();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("Demo", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n",
                SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888,
            SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL) {
        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    vscreen = (uint8_t*) malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
    if (vscreen == NULL) {
        printf("Could not allocate screen buffer\n");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    palette = (SDL_Color*) malloc(256 * sizeof(SDL_Color));
    if (palette == NULL) {
        std::cerr << "Failed to allocate palette!" << std::endl;
        free(vscreen);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

void draw_screen() {
    uint32_t now = SDL_GetTicks();
    if (prev_ticks == 0)
        prev_ticks = now;
    uint32_t sleep = 20 - (now - prev_ticks);
    prev_ticks = now;
    SDL_Delay(sleep < 0 ? 0 : sleep > 20 ? 20 : sleep);

    void *pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, &pixels, &pitch);
    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGB888);

    /*
     uint32_t *dst = (uint32_t*) pixels;
     for (int y = 0; y < SCREEN_HEIGHT; ++y) {
     for (int x = 0; x < SCREEN_WIDTH; ++x) {
     uint8_t colorIndex = vscreen[y * SCREEN_WIDTH + x];
     SDL_Color color = palette[colorIndex];
     dst[y * (pitch / 4) + x] = SDL_MapRGB(format, color.r, color.g,
     color.b);
     }
     }
     */

    uint32_t *dst = (uint32_t*) pixels;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        SDL_Color color = palette[vscreen[i]];
        dst[i] = (color.r << 16) | (color.g << 8) | color.b;
    }
    SDL_UnlockTexture(texture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_FreeFormat(format);
}

void shutdown() {
    free(vscreen);
    free(palette);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(1);
}

bool is_fullscreen = false;
void handle_events(EventHandler custom_handler) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            shutdown();
        } else if (event.type == SDL_KEYDOWN) {
            if ((event.key.keysym.sym == SDLK_ESCAPE)
                    || (event.key.keysym.sym == SDLK_q
                            && (SDL_GetModState() & KMOD_CTRL))) {
                shutdown();
            } else if (event.key.keysym.sym == SDLK_F11) {
                is_fullscreen = !is_fullscreen;
                if (is_fullscreen) {
                    SDL_SetWindowFullscreen(window,
                            SDL_WINDOW_FULLSCREEN_DESKTOP);
                } else {
                    SDL_SetWindowFullscreen(window, 0);
                }
                return;
            }
            if (custom_handler != NULL)
                custom_handler(event);
        }
    }
}
