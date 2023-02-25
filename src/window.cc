#include "window.h"

namespace emu {

int Window::x_ = 0;
int Window::y_ = 0;
int Window::w_ = 0;
int Window::h_ = 0;
int Window::scale_ = 0;

SDL_Window* Window::window_ = nullptr;
SDL_Renderer* Window::renderer_ = nullptr;
SDL_Texture* Window::texture_ = nullptr;

} // namespace emu
