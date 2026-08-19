# iv - Minimalist PPM Image Viewer

A lightweight, fast image viewer written in C using SDL2. It is designed to read Netpbm color image formats (PPM) directly from a file or piped through standard input.

## Features
* Supports both **P6 (Binary)** and **P3 (ASCII)** PPM formats.
* Flexible input: read from a file path or pipe from `stdin`.
* Minimal dependencies.

## Dependencies
* C Compiler (gcc, clang, etc.)
* SDL2 (`libsdl2-dev` on Debian/Ubuntu)

## Compilation
Compile the project using `gcc` and `sdl2-config`:

```bash
gcc -Wall -Wextra -O2 -o iv iv.c $(sdl2-config --cflags --libs)

```

## Usage

You can open an image by passing the file path as an argument:

```bash
./iv test-img.ppm

```

Or you can pipe the image data directly into the program:

```bash
cat test-img.ppm | ./iv

```

## Controls

* **Esc** or **Q**: Close the viewer.

## Roadmap

* GPU Acceleration (SDL_Renderer / SDL_Texture).
* Window scaling for images larger than the screen resolution.