#include <iostream>
#include <memory>

#include "emu.h"
#include "engine.h"

int loop(int scale, const std::string& rom_file) {
  // loop
  const int fps = 60;
  const int frame_delay = 1000 / fps; // time a frame should take in ms (1000ms / frame per second)

  Uint32 frame_start; // start of the frame
  int frame_time; // end of the frame, how long it take to process the frame

  std::unique_ptr<emu::Engine> engine{ new emu::Engine{} };

  engine->LoadRom(rom_file);
  int ret = engine->Init(
      "chip8 emulator",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      64, 32,
      scale,
      false);
  if (ret != 0) {
    return 1;
  }

  while (engine->IsRunning() == true) {
    frame_start = SDL_GetTicks();

    engine->HandleEvents();
    engine->Update();
    engine->Render();

    frame_time = SDL_GetTicks() - frame_start;
    // check if we need to delay our next frame (or we are running low on fps just render the next one)
    if(frame_delay > frame_time) {
      // delay remaining time
      SDL_Delay(frame_delay - frame_time);
    }
  }

  return 0;
}

int main(int argc, char* argv[]) {
  // args
  if (argc != 3) {
    emu::log::Error("Usage: " + std::string(argv[0]) + " SCALE ROM");
    return 1;
  }
  int scale = std::stoi(argv[1]);
  std::string rom_file = argv[2];

  int ret = 0;
  try {
    ret = loop(scale, rom_file);
  } catch (std::exception& e) {
    emu::log::Error(e.what());
  }

  return ret;
}
