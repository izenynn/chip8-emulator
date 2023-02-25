#ifndef EMU_CHIP8_H_
#define EMU_CHIP8_H_

#include <array>
#include <string>
#include <cstdint>
#include <random>

#include "log.h"

namespace emu {

class Chip8 {
  public:
    Chip8() noexcept;
    Chip8(uint16_t width, uint16_t height) noexcept;
    ~Chip8() noexcept;

    Chip8(const Chip8& rhs) = delete;
    Chip8(const Chip8&& rhs) = delete;
    Chip8& operator=(const Chip8& rhs) = delete;
    Chip8& operator=(const Chip8&& rhs) = delete;

    void LoadRom(const std::string& file);
    void Cycle();

    auto& get_keypad() { return keypad_; }
    auto& get_video() { return video_; }
  private:
    const uint16_t width_{};
    const uint16_t height_{};

    static constexpr uint16_t kEntryPointAddr = 0x200;
    static constexpr uint16_t kFontSetAddr = 0x50;

    static constexpr uint16_t kFontSetSize = 80;
    static constexpr std::array<uint8_t, kFontSetSize> kFontSet{
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	    0x20, 0x60, 0x20, 0x20, 0x70, // 1
	    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    std::array<uint8_t, 16> registers_{};
    std::array<uint8_t, 4096> memory_{};

    uint16_t index_{};
    uint16_t pc_{};

    std::array<uint16_t, 16> stack_{};
    uint8_t sp_{};

    uint8_t delay_timer_{};
    uint8_t sound_timer_{};

    std::array<uint8_t, 16> keypad_{};

    std::array<uint32_t, 64 * 32> video_{};

    uint16_t opcode_;

    //std::default_random_engine rand_gen_;
    std::mt19937 rand_gen_;
    std::uniform_int_distribution<uint8_t> rand_byte_;

    // opcodes
    void OP_00E0() noexcept;
    void OP_00EE() noexcept;

    void OP_1nnn() noexcept;
    void OP_2nnn() noexcept;
    void OP_3xkk() noexcept;
    void OP_4xkk() noexcept;
    void OP_5xy0() noexcept;
    void OP_6xkk() noexcept;
    void OP_7xkk() noexcept;

    void OP_8xy0() noexcept;
    void OP_8xy1() noexcept;
    void OP_8xy2() noexcept;
    void OP_8xy3() noexcept;
    void OP_8xy4() noexcept;
    void OP_8xy5() noexcept;
    void OP_8xy6() noexcept;
    void OP_8xy7() noexcept;
    void OP_8xyE() noexcept;

    void OP_9xy0() noexcept;
    void OP_Annn() noexcept;
    void OP_Bnnn() noexcept;
    void OP_Cxkk() noexcept;
    void OP_Dxyn() noexcept;

    void OP_Ex9E() noexcept;
    void OP_ExA1() noexcept;

    void OP_Fx07() noexcept;
    void OP_Fx0A() noexcept;
    void OP_Fx15() noexcept;
    void OP_Fx18() noexcept;
    void OP_Fx1E() noexcept;
    void OP_Fx29() noexcept;
    void OP_Fx33() noexcept;
    void OP_Fx55() noexcept;
    void OP_Fx65() noexcept;

    // function pointer table functions
    void Table0() {
      (this->*(table_0_[opcode_ & 0x000Fu]))();
    }
    void Table8() {
      (this->*(table_8_[opcode_ & 0x000Fu]))();
    }
    void TableE() {
      (this->*(table_E_[opcode_ & 0x000Fu]))();
    }
    void TableF() {
      (this->*(table_F_[opcode_ & 0x00FFu]))();
    }
    void OP_NULL() {}

    // function pointer table
    using instruction = void (Chip8::*)();
    std::array<instruction, 0xF + 1> table_; // entire opcode unique
    std::array<instruction, 0xE + 1> table_0_; // last digit unique
    std::array<instruction, 0xE + 1> table_8_; // last digit unique
    std::array<instruction, 0xE + 1> table_E_; // last digit unique
    std::array<instruction, 0x65 + 1> table_F_; // last two unique
};

} // namespace emu

#endif // EMU_CHIP8_H_
