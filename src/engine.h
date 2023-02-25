#ifndef EMU_ENGINE_H_
#define EMU_ENGINE_H_

#include <string>
#include <memory>

#include "SDL2/SDL.h"
#include "SDL2/SDL_pixels.h"

#include "window.h"
#include "chip8.h"

namespace emu {

class Engine {
  public:
    Engine();
    ~Engine();

    Engine(const Engine& rhs) = delete;
    Engine(const Engine&& rhs) = delete;
    Engine& operator=(const Engine& rhs) = delete;
    Engine& operator=(const Engine&& rhs) = delete;

    [[nodiscard]] int Init(
        const std::string& title,
        int x, int y,
        int w, int h,
        int scale,
        bool full_screen);

    void LoadRom(const std::string& file) {
      chip8_->LoadRom(file);
    }

    void HandleEvents();
    void Update();
    void Render();

    [[nodiscard]] bool IsRunning() noexcept { return running_; };
  private:
    static bool running_;

    static SDL_Event event_;

    std::unique_ptr<Chip8> chip8_;
    int video_pitch_;
};

} // namespace emu

#endif // EMU_ENGINE_H_
