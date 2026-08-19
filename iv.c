//gcc -Wall -Wextra -g -o  iv iv.c `sdl2-config --cflags --libs`
//cat test-img.ppm | ./iv OR ./iv test-img.ppm

// TODO:
//
// [ ] GPU Acceleration: Switch from SDL_Surface to SDL_Renderer and SDL_Texture.
// [ ] File reading optimization: Read P6 pixels as a single chunk using fread() instead of fgetc() (much faster).
// [ ] P3 (ASCII) support: Check the first line to identify if the file is P3 or P6, and read pixels accordingly (fscanf vs fgetc/fread).
// [x] Robust parsing: The current method of skipping comments (#) is a bit fragile. Make the header parser more robust.
// [ ] Window scaling: If the image resolution exceeds screen size (e.g., 4K), scale the window to fit.
// [x] Key bindings: Allow closing the application using the Esc or Q key (event.type == SDL_KEYDOWN).

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

// DATA STRUCTURES
typedef struct {
    int width;
    int height;
    int max_color;
    int is_p6; // 1 = P6 (Binary), 0 = P3 (ASCII)
} PPMHeader;

// FUNCTION PROTOTYPES
FILE* open_input(int argc, char *argv[]);
static void skip_whitespace_and_comments(FILE *in);
int read_ppm_header(FILE *in, PPMHeader *header);
void read_ppm_pixels(FILE *in, SDL_Surface *surface, int width, int height, int is_p6);


int main(int argc, char *argv[]) {
    FILE *in = open_input(argc, argv);
    if (!in) return 1;

    PPMHeader header;
    if (!read_ppm_header(in, &header)) {
        fprintf(stderr, "Invalid or unsupported PPM file\n");
        if (in != stdin) fclose(in);
        return 1;
    }

       if (SDL_Init(SDL_INIT_VIDEO) < 0) {
       fprintf(stderr, "could not initialize SDL! SDL_Error: %s\n", SDL_GetError());
       return 1;
   }

    SDL_Window *pwindow = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, header.width, header.height, 0); 
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
    read_ppm_pixels(in, psurface, header.width, header.height, header.is_p6);
    SDL_UnlockSurface(psurface);

    SDL_UpdateWindowSurface(pwindow);

    int app_running = 1; 
    while(app_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN &&  (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q))) {
                app_running = 0;    
            }
        }
        SDL_Delay(16);
    }
    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    
    if(in !=stdin) {
        fclose(in);
    }

    return 0;
}

FILE* open_input(int argc, char *argv[]) {
    FILE *in = stdin;
    
    if (argc > 1) {
        // opens the file in rb. (read and binary)
        in = fopen(argv[1], "rb"); 
        
        if(!in) {
            fprintf(stderr, "Error: Couldn't open file '%s'.\n", argv[1]);
            return NULL;
        }
    }
    return in;
}

static void skip_whitespace_and_comments(FILE *in) {
    // HELPER FUNCTION for going through and checking the file
    int c;
    while ((c = fgetc(in)) != EOF) {
        // if the character is space, tab, or newline, continue
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            continue;
        }
        else if (c=='#') {
            while ((c = fgetc(in)) != EOF && c != '\n');
        }
        else {
            ungetc(c, in);
            break;
        }
    }
}

int read_ppm_header(FILE *in, PPMHeader *header) {
    char magic[3];

    // read the magic word (P3 or P6)
    if(fscanf(in, "%2s", magic) != 1) {
        return 0;
    }
    
    if (magic[0] == 'P' && magic[1] == '6') {
        header->is_p6 = 1;
    } else if (magic[0] == 'P' && magic[1] == '3') {
        header->is_p6 = 0;
    } else {
        return 0;
    }
    
    skip_whitespace_and_comments(in);
    // read the width
    if(fscanf(in, "%d", &header->width) != 1) return 0;

    skip_whitespace_and_comments(in);
    // read the height
    if(fscanf(in, "%d", &header->height) != 1) return 0;
    
    skip_whitespace_and_comments(in);
    //read the max color value
    if(fscanf(in, "%d", &header->max_color) != 1) return 0;

    //read one character after the color value
    fgetc(in);

    printf("Format: P%c, Width: %d, Height: %d, Max Color: %d\n", 
           header->is_p6 ? '6' : '3', header->width, header->height, header->max_color);
           
    return 1;


}

void read_ppm_pixels(FILE *in, SDL_Surface *surface, int width, int height, int is_p6) {
  Uint32 *pixels = (Uint32 *)surface->pixels;

    if (is_p6) {
        // P6 binary format
        int total_bytes = width * height * 3;
        
        // Allocate memory for the image
        unsigned char *rgb_data = malloc(total_bytes);
        if (!rgb_data) {
            fprintf(stderr, "Failed to allocate memory.\n");
            return;
        }
        
        // read everything at once
        fread(rgb_data, 1, total_bytes, in);
    
        int i = 0;
        for(int y=0; y<height; y++) {
            for(int x=0; x<width; x++) {
                Uint8 r = rgb_data[i++];
                Uint8 g = rgb_data[i++];
                Uint8 b = rgb_data[i++];
                pixels[(y * surface->pitch / 4) + x] = SDL_MapRGB(surface->format, r, g, b);
        }
    }   
        // free the allocated memory
        free(rgb_data);
        
    } else {
        // P3: ASCII format
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int r, g, b;

                if (fscanf(in, "%d %d %d", &r, &g, &b) == 3) {
                    pixels[(y * surface->pitch / 4) + x] = SDL_MapRGB(surface->format, (Uint8)r, (Uint8)g, (Uint8)b);
                }
            }
        }
    }
}