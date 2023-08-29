/*
** Matt Semmel
** Project 1 - Fall 2022
*/

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "graphics.h"

int main()
{
  color_t colorWheel[14] = 
  {
    RGB(31, 0, 0),
    RGB(31, 36, 0),
    RGB(31, 63, 0),
    RGB(0, 33, 0),
    RGB(0, 21, 31),
    RGB(15, 0, 17),
    RGB(30, 32, 0),
    RGB(31, 0, 0),
    RGB(0, 63, 0),
    RGB(0, 0, 31),
    RGB(31, 63, 0),
    RGB(0, 63, 31),
    RGB(31, 0, 31),
    RGB(0, 63, 0)
  };
  
  color_t default_text = RGB(31, 63, 31);

  init_graphics();
  void* fb = new_offscreen_buffer();

  int i;

  for ( i = 0; i < 15; i++ )
  {
    draw_text ( fb, 50, 100, "Welcome to Matt's driver.c!\nPress 'c' to continue!\nPress 'q' to quit!", colorWheel[i] );
    sleep_ms(100000000);
    
    blit(fb);
  }

  if ( getkey() == 'c' )
  {
    clear_screen(fb);
    blit(fb);

    draw_text ( fb, 25, 25, "Oh wow! Here we go!", default_text );
    sleep_ms(5000000);
    sleep_ms(5000000);
    sleep_ms(5000000);
    blit(fb);

    draw_text ( fb, 50, 100, "It's gonna draw some lines now, k?", default_text );
    sleep_ms(5000000);
    sleep_ms(5000000);
    sleep_ms(5000000);
    blit(fb);


    sleep_ms(5000000);
    sleep_ms(5000000);
    clear_screen(fb);
    blit(fb);

    draw_line ( fb, 60, 60, 267, 267, colorWheel[1] );
    sleep_ms(5000000);
    blit(fb);

    draw_line ( fb, 61, 61, 100, 60, colorWheel[2] );
    sleep_ms(5000000);
    blit(fb);

    draw_line ( fb, 88, 66, 167, 167, colorWheel[3] );
    sleep_ms(5000000);
    blit(fb);

    draw_line ( fb, 100, 100, 300, 300, colorWheel[4] );
    sleep_ms(5000000);
    blit(fb);

    draw_line ( fb, 156, 222, 89, 66, colorWheel[5] );
    sleep_ms(5000000);
    blit(fb);

    sleep_ms(10000000);
    draw_text ( fb, 25, 25, "Oh cool. Those were lines drawn by drawing pixels.", default_text );
    draw_text ( fb, 25, 40, "Program will exit itself shortly, k?", default_text );
    blit(fb);

    sleep_ms(300000000);
    sleep_ms(300000000);
    sleep_ms(300000000);
    sleep_ms(300000000);

    clear_screen(fb);
    exit_graphics();

    return 0;
  }  

  else if ( getkey() == 'q' )
  {
    clear_screen(fb);
    exit_graphics();
  
    return 0;
  }
}