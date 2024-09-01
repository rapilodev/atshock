#ifndef MCGA_H
#define MCGA_H

#include <stdlib.h>
#include <stdint.h>
#include <functional>
#include <SDL2/SDL.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

// Define screen dimensions
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

uint8_t* get_vscreen();
SDL_Color* get_palette();
SDL_Renderer* get_renderer();
SDL_Texture* get_texture();
SDL_Window* get_window();

void init();
void draw_screen();
void copy_screen(uint8_t * source, uint8_t * target);
void clear_screen();
void set_pal(int n, uint8_t r, uint8_t g, uint8_t b);
void set_fullpal(uint8_t *pal);
void shutdown();

using EventHandler = std::function<void(const SDL_Event&)>;
void handle_events(EventHandler custom_handler);

void mousemove(int *x, int *y);
void init_random(void);
int get_random(uint max);
int get_random_int(uint max);
void softscreen(uint8_t *where);
void softflip(uint8_t *src, uint8_t *dest);
void put_pixel(uint x, uint y, uint8_t color, uint8_t *screen);
void copy_pixel(uint8_t *source, uint8_t *target, uint32_t sadd, uint32_t tadd);
void line(int x1, int y1, int x2, int y2, uint8_t col, uint8_t *where);
void circle(int xm, int ym, int r, int col, uint8_t *where);
void rect(int x1, int y1, int x2, int y2, int c, uint8_t *screen);
void polrect(int x1, int y1, int x2, int y2, int c1, int c2, uint8_t *where);
void polrect2(int x1, int y1, int x2, int y2, int c1, int c2, int d1, int d2,
        uint8_t *where);
void plasma(int x1, int y1, int len, int rough, uint8_t *where);
void fill_circle(int xm, int ym, int r, int col, uint8_t *where);
void read_pcx(const char *pcxname, uint8_t *screen, uint8_t *pal);
long int wurzel(long int a);

#endif // MCGA_H
