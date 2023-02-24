#include <iostream>

#include "SDL2/SDL.h"

#include "log.h"

int main() {
  const int fps = 60;
  const int frame_delay = 1000 / fps; // time a frame should take in ms (1000ms / frame per second)

  Uint32 frame_start; // start of the frame
  int frame_time; // end of the frame, how long it take to process the frame

  // init sdl
  if (SDL_Init(SDL_INIT_VIDEO) > 0) {
    emu::log::SdlError("SDL_Init failed!");
    return 1;
  }

  // create window
  Uint32 sdl_flags = SDL_WINDOW_SHOWN;
  SDL_Window* window = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, sdl_flags);
  if (window == nullptr) {
    emu::log::SdlError("SDL_CreateWindow failed!");
    return 1;
  }

  // create renderer
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    emu::log::SdlError("SDL_CreateRenderer failed!");
    return 1;
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  bool running = true;
  while (running) {
    frame_start = SDL_GetTicks();

    // events
    SDL_Event event;
    SDL_PollEvent(&event);
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) running = false;
    }
    // update
    // render
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    frame_time = SDL_GetTicks() - frame_start;

    // check if we need to delay our next frame (or we are running low on fps just render the next one)
    if(frame_delay > frame_time) {
      // delay remaining time
      SDL_Delay(frame_delay - frame_time);
    }
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return 0;
}
