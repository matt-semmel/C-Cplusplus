/*
** Matt Semmel
** Project 1 - Fall 2022
*/

#include "graphics.h"
#include "iso_font.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

typedef enum { false, true } bool;

static int fb;
static void* fb_mem;
static int fb_size;
static int line_count;
static int line_length;

struct fb_fix_screeninfo fix_info;
struct fb_var_screeninfo var_info;

/* Uses: open, ioctl, mmap */
void init_graphics()
{  
  struct termios term;

  /* Set the flag to "read/write". Derp. */
  fb = open("/dev/fb0", O_RDWR);

  if ( fb == -1 )
    return;

  if ( ioctl(fb, FBIOGET_FSCREENINFO, &fix_info) == -1 )
    return;
  
  if ( ioctl(fb, FBIOGET_VSCREENINFO, &var_info) == -1 )
    return;
  
  /* Determine resolution/depth and maps it to memory */
  line_count = var_info.yres_virtual;
  line_length = fix_info.line_length / 2;
  fb_size = line_count * line_length * 2;
  fb_mem = mmap( NULL, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0 );
  
  if ( fb_mem == MAP_FAILED )
    return;
  
  write( STDOUT_FILENO, "\033[2J", 4 );
  
  /* Disable canonical mode and echo. */
  if (ioctl(STDIN_FILENO, TCGETS, &term) == -1)
    return;
  
  term.c_lflag &= ~(ICANON | ECHO);
  
  if (ioctl(STDIN_FILENO, TCSETS, &term) == -1)
    return;
  
}

/* Uses: ioctl */
void exit_graphics()
{
  struct termios term;

  /* 
  ** Don't assume that file will close and memory will be unmapped on its own.
  ** It feels like good practice to tell the computer to do this.
  */
  if (munmap(fb_mem, fb_size) == -1)
    return;
  
  if (close(fb) == -1)
    return;
  
  /* Reenable canonical mode and echo. */
  if (ioctl(STDIN_FILENO, TCGETS, &term) == -1)
    return;
  
  term.c_lflag |= (ICANON | ECHO);
  
  if (ioctl(STDIN_FILENO, TCSETS, &term) == -1)
    return;
}

/* Uses: select, read */
char getkey()
{
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  char key;

  if (select(STDIN_FILENO + 1, &fds, NULL, NULL, NULL) > 0 && read(STDIN_FILENO, &key, 1) > 0)
  {
    return key;
  }
  
  return 0;
}

/* Uses: nanosleep */
void sleep_ms ( long ms )
{
  struct timespec time;
  time.tv_sec = 0;
  time.tv_nsec = ms*(1000000);

  nanosleep ( &time, NULL );
}

/* No system calls */
void clear_screen ( void* img )
{
  int x, y;
  
  for(y = 0; y < var_info.yres_virtual; y++)
  {
    for(x = 0; x < fix_info.line_length/2; x++)
    {
      draw_pixel((color_t*)img, x, y, (color_t) 0);
    }
  }

  write(1, "\033[2J", 7);
  
  return;
}

/* No system calls */
void draw_pixel ( void* img, int x, int y, color_t c )
{

  /* Stops function from drawing outside teh map */
  if ( x < 0 || y < 0 || x >= line_length || y >= line_count )
  {
    return;
  }

  else
  {
    int offset = y * line_length + x;
    *( (color_t*)(img) + offset ) = c;
  }
}

/* 
** No system calls
** Per instructions...
** Source: http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
*/
void draw_line ( void* img, int x1, int y1, int x2, int y2, color_t c )
{ 
  int x = x1;
  int y = y1;
  int w = x2 - x1;
  int h = y2 - y1;

  int dx1 = w >= 0 ? 1 : -1;
  int dy1 = h >= 0 ? 1 : -1;
  int dx2 = dx1;
  int dy2 = 0;

  int longest = w > 0 ? w : -w;
  int shortest = h > 0 ? h : -h;
  
  if ( longest <= shortest )
  {
    int tmp = longest;
    longest = shortest;
    shortest = tmp;
    dy2 = dy1;
    dx2 = 0;
  }

  int numerator = longest >> 1;
  int i;

  for ( i = 0; i <= longest; i++ )
  {
    draw_pixel( img, x, y, c );
    numerator += shortest;

    if ( numerator >= longest )
    {
      numerator -= longest;
      x += dx1;
      y += dy1;
    }
    
    else
    {
      x += dx2;
      y += dy2;
    }
  }
}

/* Per instructions, helper function to draw the chars for draw_text */
void draw_char( void* img, int x, int y, char ch, color_t c )
{
  int letter = (int) ch;
  int i = 0;
  int j = 0;
  
  for( i = 0; i < 16; i++ )
  {
    unsigned char row = iso_font[letter*16+i];
    
    for( j = 0; j < 8; j++ )
    {
      if( (row >> (j)) & 1 == 1 )
      {
        draw_pixel( img, x+j, y+i, c );
      }
    }
  }
}

/* No system calls */
void draw_text( void* img, int x, int y, const char *text, color_t c )
{
  int counter = 0;
  
  while( text[counter] != '\0' )
  {
    draw_char( img, x, y, text[counter], c );
    x += 8;
    counter++;
  }
}

/* Uses: mmap */
void* new_offscreen_buffer ()
{
  void* offscreen_buff = mmap( NULL, fb_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
  
  /* Checks the flag to make sure mapping didn't fail. */
  if ( offscreen_buff == MAP_FAILED )
    return NULL;
  
  return offscreen_buff;
}

/* No system calls */
void blit ( void *src )
{
  size_t i;
  
  for ( i = 0; i < fb_size; i++ )
  {
    *( (char*)(fb_mem) + i ) = *( (char*)(src) + i );
  }
}