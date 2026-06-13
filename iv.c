#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
int main() {
    FILE *in = stdin;
    char *pthrowaway = calloc(1000, sizeof(char));
    // Read the first line (specifier P3 or P6 - ignore here)
    fgets(pthrowaway, 1000, in);

    // Read the second line (comment)
    fgets(pthrowaway, 1000, in);
    // Read the third line (dimensions: width / height)
    char *pdimensions = calloc(1000, sizeof(char));
    fgets(pdimensions, 1000, in);
    // Read the fourth line (max color value - ignore it)
    fgets(pthrowaway, 1000, in);
    free(pthrowaway);
    int width = -1;
    int height = -1;
    sscanf(pdimensions, "%d %d\n", &width, &height); 
    free(pdimensions);
    printf("width=%d, height=%d\n", width, height);


    SDL_Window *pwindow = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0); 
    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);

    Uint8 r, g, b;
    r=0xFF;
    g=0x00;
    b=0x00;
    Uint32 color = SDL_MapRGB(psurface->format, r,g,b);

    SDL_Rect pixel = (SDL_Rect){0,0,1,1};
    for(int x=0; x<width; x++) {
        for(int y=0; y<height; y++) {
            pixel.x=x;
            pixel.y=y; 
            SDL_FillRect(psurface, &pixel, color); 
            
        }
    }
    SDL_UpdateWindowSurface(pwindow);
    SDL_Delay(3000);
}
