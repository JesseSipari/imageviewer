//gcc -Wall -Wextra -g -o  iv iv.c `sdl2-config --cflags --libs`
//cat test-img.ppm | ./iv
// TODO
// Optimize to write values to the GPU memory instead of using SDL_Rect
// P3 or P6 files? P6 = Binary, P3 = ASCII

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    FILE *in = stdin;
    
    if (argc > 1) {
        // opens the file in rb. (read and binary)
        in = fopen(argv[1], "rb"); 
        
        if(!in) {
            fprintf(stderr, "Error: Couldn't open file '%s'.\n", argv[1]);
            return 1;
        }
    }
    
    char buffer[1000];
    // Read the first line (specifier P3 or P6 - ignore here)
    fgets(buffer, sizeof(buffer), in);

    // Skip the comments(#) and find the dimensions 
    do {
        fgets(buffer, sizeof(buffer), in);
    } while (buffer[0] == '#');
    int width = -1;
    int height = -1;
    sscanf(buffer, "%d %d", &width, &height); 
    printf("width=%d, height=%d\n", width, height);

    // Skip comments and find the max color value
    do {
        fgets(buffer, sizeof(buffer), in);
    } while (buffer[0] == '#');

   if(SDL_Init(SDL_INIT_VIDEO) < 0) {
       fprintf(stderr, "could not initialize SDL! SDL_Error: %s\n", SDL_GetError());
       return 1;
   }

    SDL_Window *pwindow = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0); 
    if (!pwindow) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);
    if(!psurface) {
        fprintf(stderr, "Failed to get surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(pwindow);
        return 1;
    }
    
    SDL_LockSurface(psurface);

    Uint32 *pixels = (Uint32 *)psurface->pixels;

    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            Uint8 r, g, b;
            r=(Uint8) fgetc(in);
            g=(Uint8) fgetc(in);
            b=(Uint8) fgetc(in);
            
            pixels[(y * psurface->pitch / 4) + x] = SDL_MapRGB(psurface->format, r, g, b);
        }
    }
    SDL_UnlockSurface(psurface);

    SDL_UpdateWindowSurface(pwindow);
    int app_running = 1; 
    while(app_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                app_running = 0;    
            }
        }
        SDL_Delay(100);
    }
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    
    if(in !=stdin) {
        fclose(in);
    }

    return 0;
}
