#include "engine.h"

namespace emu {

SDL_Event Engine::event_{};
bool Engine::running_ = false;

Engine::Engine()
    : chip8_(new Chip8{64, 32}) {}

Engine::~Engine() {
  if (Window::window_ != nullptr) {
    SDL_DestroyWindow(Window::window_);
  }
  if (Window::renderer_ != nullptr) {
    SDL_DestroyRenderer(Window::renderer_);
  }
  if (Window::texture_ != nullptr) {
    SDL_DestroyTexture(Window::texture_);
  }

  SDL_Quit();
}

int Engine::Init(const std::string& title, int x, int y, int w, int h, int scale, bool full_screen) {
  // set window value
  Window::x_ = x;
  Window::y_ = y;
  Window::w_ = w;
  Window::h_ = h;
  Window::scale_ = scale;

  // init sdl
  if (SDL_Init(SDL_INIT_VIDEO) > 0) {
    emu::log::SdlError("SDL_Init failed!");
    return 1;
  }

  // create window
  Uint32 sdl_flags = SDL_WINDOW_SHOWN;
  if (full_screen == true) {
    sdl_flags |= SDL_WINDOW_FULLSCREEN;
  }
  Window::window_ = SDL_CreateWindow(
      title.c_str(),
      Window::x_, Window::y_,
      Window::w_ * Window::scale_, Window::h_ * Window::scale_,
      sdl_flags);
  if (Window::window_ == nullptr) {
    log::SdlError("SDL_CreateWindow failed!");
    return 1;
  }

  // create renderer
  Window::renderer_ = SDL_CreateRenderer(
      Window::window_,
      -1,
      SDL_RENDERER_ACCELERATED);
  if (Window::renderer_ == nullptr) {
    log::SdlError("SDL_CreateRenderer failed!");
    return 1;
  }
  SDL_SetRenderDrawColor(Window::renderer_, 0, 0, 0, 255);
  // create texture
  Window::texture_ = SDL_CreateTexture(
      Window::renderer_,
      SDL_PIXELFORMAT_RGBA8888,
      SDL_TEXTUREACCESS_STREAMING,
      Window::w_, Window::h_);
  if (Window::texture_ == nullptr) {
    log::SdlError("SDL_CreateRenderer failed!");
    return 1;
  }

  // set pitch
  video_pitch_ = sizeof(chip8_->get_video()[0]) * Window::w_;

  // ready to run!
  running_ = true;

  return 0;
}

void Engine::HandleEvents() {
  while (SDL_PollEvent(&event_)) {
    if (event_.type == SDL_QUIT) {
      running_ = false;
    } else if (event_.type == SDL_KEYDOWN) {
      switch (event_.key.keysym.sym) {
        case SDLK_ESCAPE:
          running_ = false; break;
        case SDLK_x:
          chip8_->get_keypad()[0x0] = 1; break;
        case SDLK_1:
          chip8_->get_keypad()[0x1] = 1; break;
        case SDLK_2:
          chip8_->get_keypad()[0x2] = 1; break;
        case SDLK_3:
          chip8_->get_keypad()[0x3] = 1; break;
        case SDLK_q:
          chip8_->get_keypad()[0x4] = 1; break;
        case SDLK_w:
          chip8_->get_keypad()[0x5] = 1; break;
        case SDLK_e:
          chip8_->get_keypad()[0x6] = 1; break;
        case SDLK_a:
          chip8_->get_keypad()[0x7] = 1; break;
        case SDLK_s:
          chip8_->get_keypad()[0x8] = 1; break;
        case SDLK_d:
          chip8_->get_keypad()[0x9] = 1; break;
        case SDLK_z:
          chip8_->get_keypad()[0xA] = 1; break;
        case SDLK_c:
          chip8_->get_keypad()[0xB] = 1; break;
        case SDLK_4:
          chip8_->get_keypad()[0xC] = 1; break;
        case SDLK_r:
          chip8_->get_keypad()[0xD] = 1; break;
        case SDLK_f:
          chip8_->get_keypad()[0xE] = 1; break;
        case SDLK_v:
          chip8_->get_keypad()[0xF] = 1; break;
      }
    } else if (event_.type == SDL_KEYUP) {
      switch (event_.key.keysym.sym) {
        case SDLK_x:
          chip8_->get_keypad()[0x0] = 0; break;
        case SDLK_1:
          chip8_->get_keypad()[0x1] = 0; break;
        case SDLK_2:
          chip8_->get_keypad()[0x2] = 0; break;
        case SDLK_3:
          chip8_->get_keypad()[0x3] = 0; break;
        case SDLK_q:
          chip8_->get_keypad()[0x4] = 0; break;
        case SDLK_w:
          chip8_->get_keypad()[0x5] = 0; break;
        case SDLK_e:
          chip8_->get_keypad()[0x6] = 0; break;
        case SDLK_a:
          chip8_->get_keypad()[0x7] = 0; break;
        case SDLK_s:
          chip8_->get_keypad()[0x8] = 0; break;
        case SDLK_d:
          chip8_->get_keypad()[0x9] = 0; break;
        case SDLK_z:
          chip8_->get_keypad()[0xA] = 0; break;
        case SDLK_c:
          chip8_->get_keypad()[0xB] = 0; break;
        case SDLK_4:
          chip8_->get_keypad()[0xC] = 0; break;
        case SDLK_r:
          chip8_->get_keypad()[0xD] = 0; break;
        case SDLK_f:
          chip8_->get_keypad()[0xE] = 0; break;
        case SDLK_v:
          chip8_->get_keypad()[0xF] = 0; break;
      }
    }
  }
}

void Engine::Update() {
  chip8_->Cycle();
  SDL_UpdateTexture(
      Window::texture_,
      nullptr,
      static_cast<void*>(chip8_->get_video().data()),
      video_pitch_);
}

void Engine::Render() {
  SDL_RenderClear(Window::renderer_);

  SDL_RenderCopy(Window::renderer_, Window::texture_, nullptr, nullptr);

  SDL_RenderPresent(Window::renderer_);
}

} // namespace emu
