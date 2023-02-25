#ifndef EMU_WINDOW_H_
#define EMU_WINDOW_H_

#include "emu.h"

namespace emu {

class Window {
  public:
    Window() = delete;
    Window(const Window& rhs) = delete;
    Window(const Window&& rhs) = delete;
    ~Window() = delete;
    Window& operator=(const Window& rhs) = delete;
    Window& operator=(const Window&& rhs) = delete;

    static SDL_Window* get_window() { return window_; };
    static SDL_Renderer* get_renderer() { return renderer_; };

    friend class Engine;
  private:
    static int x_;
    static int y_;
    static int w_;
    static int h_;
    static int scale_;

    static SDL_Window* window_;
    static SDL_Renderer* renderer_;
    static SDL_Texture* texture_;
};

} // namespace emu

#endif // EMU_VECTOR2_H_
