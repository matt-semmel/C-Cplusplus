/*
** Matt Semmel
** Project 1 - Fall 2022
*/

typedef unsigned short int color_t;

#define RGB( R,G,B ) ( (color_t) ((R<<11) + ((G&0x003f)<<5) + (B&0x001f)) )

void init_graphics();
void exit_graphics();
char getKey();
void sleep_ms( long ms );
void clear_screen( void *img );
void draw_pixel( void *img, int x, int y, color_t color );
void draw_line( void *img, int x1, int y1, int x2, int y2, color_t c );
void draw_char( void *img, int x, int y, char ch, color_t c );
void draw_text( void *img, int x, int y, const char *text, color_t c );
void* new_offscreen_buffer();
void blit( void *src );