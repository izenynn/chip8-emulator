#include <cstdint>
#include <cstring>
#include <fstream>
#include <memory>
#include <chrono>

#include "chip8.h"

namespace emu {

Chip8::Chip8() noexcept
    : Chip8(64, 32) {}

Chip8::Chip8(uint16_t width, uint16_t height) noexcept
    : width_(width), height_(height),
      pc_(kEntryPointAddr),
      rand_gen_(std::chrono::system_clock::now().time_since_epoch().count()) {

  // load fonts into memory
  for (uint16_t i = 0; i < kFontSetSize; ++i) {
    memory_[kFontSetAddr + i] = kFontSet[i];
  }

  // initialize RNG
  rand_byte_ = std::uniform_int_distribution<uint8_t>(0u, 255u);

  // set up function pointer table
  table_[0x0] = &Chip8::Table0;
  table_[0x1] = &Chip8::OP_1nnn;
  table_[0x2] = &Chip8::OP_2nnn;
  table_[0x3] = &Chip8::OP_3xkk;
  table_[0x4] = &Chip8::OP_4xkk;
  table_[0x5] = &Chip8::OP_5xy0;
  table_[0x6] = &Chip8::OP_6xkk;
  table_[0x7] = &Chip8::OP_7xkk;
  table_[0x8] = &Chip8::Table8;
  table_[0x9] = &Chip8::OP_9xy0;
  table_[0xA] = &Chip8::OP_Annn;
  table_[0xB] = &Chip8::OP_Bnnn;
  table_[0xC] = &Chip8::OP_Cxkk;
  table_[0xD] = &Chip8::OP_Dxyn;
  table_[0xE] = &Chip8::TableE;
  table_[0xF] = &Chip8::TableF;

  //for (auto& value : table_0_) value = &Chip8::OP_NULL;
  std::fill(std::begin(table_0_), std::end(table_0_), &Chip8::OP_NULL);
  std::fill(std::begin(table_8_), std::end(table_8_), &Chip8::OP_NULL);
  std::fill(std::begin(table_E_), std::end(table_E_), &Chip8::OP_NULL);
  std::fill(std::begin(table_F_), std::end(table_F_), &Chip8::OP_NULL);

  table_0_[0x0] = &Chip8::OP_00E0;
  table_0_[0xE] = &Chip8::OP_00EE;

  table_8_[0x0] = &Chip8::OP_8xy0;
  table_8_[0x1] = &Chip8::OP_8xy1;
  table_8_[0x2] = &Chip8::OP_8xy2;
  table_8_[0x3] = &Chip8::OP_8xy3;
  table_8_[0x4] = &Chip8::OP_8xy4;
  table_8_[0x5] = &Chip8::OP_8xy5;
  table_8_[0x6] = &Chip8::OP_8xy6;
  table_8_[0x7] = &Chip8::OP_8xy7;
  table_8_[0xE] = &Chip8::OP_8xyE;

  table_E_[0x1] = &Chip8::OP_ExA1;
  table_E_[0xE] = &Chip8::OP_Ex9E;

  table_F_[0x07] = &Chip8::OP_Fx07;
  table_F_[0x0A] = &Chip8::OP_Fx0A;
  table_F_[0x15] = &Chip8::OP_Fx15;
  table_F_[0x18] = &Chip8::OP_Fx18;
  table_F_[0x1E] = &Chip8::OP_Fx1E;
  table_F_[0x29] = &Chip8::OP_Fx29;
  table_F_[0x33] = &Chip8::OP_Fx33;
  table_F_[0x55] = &Chip8::OP_Fx55;
  table_F_[0x65] = &Chip8::OP_Fx65;
}

Chip8::~Chip8() noexcept {}

void Chip8::LoadRom(const std::string& file) {
  std::ifstream fs{file.c_str(), std::ios::binary | std::ios::ate};

  // open ROM as a binary stream and load it
  if (fs.is_open()) {
    std::streampos size = fs.tellg();
    std::unique_ptr<char[]> buffer { new char[size] };

    fs.seekg(0, std::ios::beg);
    fs.read(buffer.get(), size);
    fs.close();

    for (uint16_t i = 0; i < size; ++i) {
      memory_[kEntryPointAddr + i] = buffer[i];
    }
  } else {
    throw std::runtime_error("can't open ROM file: " + file);
  }
  fs.close();
}

void Chip8::Cycle() {
  // fetch
  opcode_ = (memory_[pc_] << 8u) | memory_[pc_ + 1];
  // increment pc before executing
  pc_ += 2;
  // decode and execute
  (this->*(table_[(opcode_ & 0xF000u) >> 12u]))();
  // decrement delay timer
  if (delay_timer_ > 0) {
    --delay_timer_;
  }
  // decrement sound timer
  if (sound_timer_ > 0) {
    --sound_timer_;
  }
}

// opcodes

// 00E0: CLS
// Clear the display
void Chip8::OP_00E0() noexcept {
  memset(video_.data(), 0, video_.size());
}
// 00EE: RET
// Return from a subroutine
void Chip8::OP_00EE() noexcept {
  --sp_;
  pc_ = stack_[sp_];
}
// 1nnn: JP addr
// Jump to location nnn, the interpreter sets the program counter to nnn
void Chip8::OP_1nnn() noexcept {
  uint16_t addr = opcode_ & 0x0FFFu;
  pc_ = addr;
}
// 2nnn: CALL addr
// Call subroutine at nnn
void Chip8::OP_2nnn() noexcept {
  uint16_t addr = opcode_ & 0x0FFFu;
  stack_[sp_] = pc_;
  ++sp_;
  pc_ = addr;
}
// 3xkk: SE Vx, byte
// Skip next instruction if Vx == kk
void Chip8::OP_3xkk() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t byte = opcode_ & 0x00FFu;
  if (registers_[Vx] == byte) {
    pc_ += 2;
  }
}
// 4xkk: SNE Vx, byte
// Skip next instruction if Vx != kk
void Chip8::OP_4xkk() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t byte = opcode_ & 0x00FFu;
  if (registers_[Vx] != byte) {
    pc_ += 2;
  }
}
// 5xy0: SE Vx, Vy
// Skip next instruction if Vx == Vy
void Chip8::OP_5xy0() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  if (registers_[Vx] == registers_[Vy]) {
    pc_ += 2;
  }
}
// 6xkk: LD Vx, byte
// Set Vx = kk
void Chip8::OP_6xkk() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t byte = opcode_ & 0x00FFu;
  registers_[Vx] = byte;
}
// 7xkk: ADD Vx, byte
// Set Vx = Vx + kk
void Chip8::OP_7xkk() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t byte = opcode_ & 0x00FFu;
  registers_[Vx] += byte;
}
// 8xy0: LD Vx, Vy
// Set Vx = Vy
void Chip8::OP_8xy0() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  registers_[Vx] = registers_[Vy];
}
// 8xy1: OR Vx, Vy
// Set Vx = Vx OR Vy
void Chip8::OP_8xy1() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  registers_[Vx] |= registers_[Vy];
}
// 8xy2: AND Vx, Vy
// Set Vx = Vx AND Vy
void Chip8::OP_8xy2() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  registers_[Vx] &= registers_[Vy];
}
// 8xy3: XOR Vx, Vy
// Set Vx = Vx XOR Vy
void Chip8::OP_8xy3() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  registers_[Vx] ^= registers_[Vy];
}
// 8xy4: ADD Vx, Vy
// Set Vx = Vx + Vy, set VF = carry
// The values of Vx and Vy are added together. If the result is greater than 8
// bits (i.e., > 255) VF is set to 1, otherwise 0. Only the lowest 8 bits of
// the result are kept, and stored in Vx
void Chip8::OP_8xy4() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  uint16_t sum = registers_[Vx] + registers_[Vy];
  if (sum > 255u) {
    registers_[0xF] = 1;
  } else {
    registers_[0xF] = 0;
  }
  registers_[Vx] = sum & 0xFFu;
}
// 8xy5: SUB Vx, Vy
// Set Vx = Vx - Vy. set VF = NOT borrow
// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx,
// and the results stored in Vx
void Chip8::OP_8xy5() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  if (registers_[Vx] > registers_[Vy]) {
    registers_[0xF] = 1;
  } else {
    registers_[0xF] = 0;
  }
  registers_[Vx] -= registers_[Vy];
}
// 8xy6: SHR Vx
// Set Vx = Vx SHR 1
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
void Chip8::OP_8xy6() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  registers_[0xF] = (registers_[Vx] & 0x1u);
  registers_[Vx] >>= 1;
}
// 8xy7: SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = NOT borrow
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy,
// and the results stored in Vx
void Chip8::OP_8xy7() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  if (registers_[Vy] > registers_[Vx]) {
    registers_[0xF] = 1;
  } else {
    registers_[0xF] = 0;
  }
  registers_[Vx] = registers_[Vy] - registers_[Vx];
}
// 8xyE: SHR Vx {, Vy}
// Set Vx = Vx SHL 1
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise 0
void Chip8::OP_8xyE() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  registers_[0xF] = (registers_[Vx] & 0x80u) >> 7u;
  registers_[Vx] <<= 1;
}
// 9xyE: SNE Vx, Vy
// Skip next instruction if Vx != Vy
void Chip8::OP_9xy0() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  if (registers_[Vx] != registers_[Vy]) {
    pc_ += 2;
  }
}
// Annn: LD I, addr
// Set I = nnn
void Chip8::OP_Annn() noexcept {
  uint16_t addr = opcode_ & 0x0FFFu;
  index_ = addr;
}
// Bnnn: JP V0, addr
// Jump to location nnn + V0
void Chip8::OP_Bnnn() noexcept {
  uint16_t addr = opcode_ & 0x0FFFu;
  pc_ = registers_[0x0] + addr;
}
// Cxkk: RND Vx, byte
// Set Vx = random byte AND kk
void Chip8::OP_Cxkk() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t byte = opcode_ & 0x00FFu;
  registers_[Vx] = rand_byte_(rand_gen_) & byte;
}
// Dxyn: DRW Vx, Vy, nibble
// Display n-byte sprite starting ot memory location I at (Vx, Vy), set VF = collision
void Chip8::OP_Dxyn() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t Vy = (opcode_ & 0x00F0u) >> 4u;
  uint8_t height = opcode_ & 0x000Fu;

  //// wrap if going beyond screen boundaries
  uint8_t x = registers_[Vx] % width_;
  uint8_t y = registers_[Vy] % height_;
  //uint8_t x = registers_[Vx];
  //uint8_t y = registers_[Vy];

  registers_[0xF] = 0;

  for (uint16_t row = 0; row < height; ++row) {
    uint8_t sprite_byte = memory_[index_ + row];
    for (uint16_t col = 0; col < 8; ++col) {
      uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
      // wrap if going beyond screen boundaries
      x = x % width_;
      y = y % height_;
      uint32_t* screen_pixel = &video_[(y + row) * width_ + (x + col)];
      // sprite pixel on
      if (sprite_pixel != 0u) {
        // screen pixel also on -> collision
        if (*screen_pixel == 0xFFFFFFFFu) {
          registers_[0xF] = 1;
        }
        // effectively XOR with the sprite pixel
        *screen_pixel ^= 0xFFFFFFFFu;
      }
    }
  }
}
// Ex9E: SKP Vx
// Skip next instruction if key with the value of Vx is pressed
void Chip8::OP_Ex9E() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t key = registers_[Vx];
  if (keypad_[key]) {
    pc_ += 2;
  }
}
// ExA1: SKNP Vx
// Skip next instruction if key with the value of Vx is not pressed
void Chip8::OP_ExA1() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t key = registers_[Vx];
  if (!keypad_[key]) {
    pc_ += 2;
  }
}
// Fx07: LD Vx, DT
// Set Vx = delay timer value
void Chip8::OP_Fx07() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  registers_[Vx] = delay_timer_;
}
// Fx0A: LD Vx, K
// Wait for a key press, store the value of the key in Vx
void Chip8::OP_Fx0A() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  if (keypad_[0x0]) {
    registers_[Vx] = 0x0;
  } else if (keypad_[0x1]) {
    registers_[Vx] = 0x1;
  } else if (keypad_[0x2]) {
    registers_[Vx] = 0x2;
  } else if (keypad_[0x3]) {
    registers_[Vx] = 0x3;
  } else if (keypad_[0x4]) {
    registers_[Vx] = 0x4;
  } else if (keypad_[0x5]) {
    registers_[Vx] = 0x5;
  } else if (keypad_[0x6]) {
    registers_[Vx] = 0x6;
  } else if (keypad_[0x7]) {
    registers_[Vx] = 0x7;
  } else if (keypad_[0x8]) {
    registers_[Vx] = 0x8;
  } else if (keypad_[0x9]) {
    registers_[Vx] = 0x9;
  } else if (keypad_[0xA]) {
    registers_[Vx] = 0xA;
  } else if (keypad_[0xB]) {
    registers_[Vx] = 0xB;
  } else if (keypad_[0xC]) {
    registers_[Vx] = 0xC;
  } else if (keypad_[0xD]) {
    registers_[Vx] = 0xD;
  } else if (keypad_[0xE]) {
    registers_[Vx] = 0xE;
  } else if (keypad_[0xF]) {
    registers_[Vx] = 0xF;
  } else {
    pc_ -= 2;
  }
}
// Fx15: LD DT, Vx
// Set delay timer = Vx
void Chip8::OP_Fx15() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  delay_timer_ = registers_[Vx];
}
// Fx18: LD ST, Vx
// Set sound time = Vx
void Chip8::OP_Fx18() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  sound_timer_ = registers_[Vx];
}
// Fx1E: ADD, I, Vx
// Set I = I + Vx
void Chip8::OP_Fx1E() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  index_ += registers_[Vx];
}
// Fx29: LD F, Vx
// Set I = location of sprite for digit Vx
void Chip8::OP_Fx29() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  index_ = kFontSetAddr + (5 * registers_[Vx]);
}
// Fx33: LD B, Vx
// Store BCD representation of Vx in memory locations I, I+1 and I+2
// The interpreter takes the decimal value of Vx, and places the hundreds digit
// in memory at location in I, the tens digit at location I+1, and the ones
// digit at location I+2
void Chip8::OP_Fx33() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  uint8_t value = registers_[Vx];
  // ones
  memory_[index_ + 2] = value % 10;
  value /= 10;
  // tens
  memory_[index_ + 1] = value % 10;
  value /= 10;
  // hundreds
  memory_[index_] = value % 10;
}
// LD [I], Vx
// Stare registers V0 through Vx in memory starting at location I
void Chip8::OP_Fx55() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i) {
    memory_[index_ + i] = registers_[i];
  }
}
// LD Vx, [I]
// Read registers V0 through Vx from memory starting at location I
void Chip8::OP_Fx65() noexcept {
  uint8_t Vx = (opcode_ & 0x0F00u) >> 8u;
  for (uint8_t i = 0; i <= Vx; ++i) {
    registers_[i] = memory_[index_ + i];
  }
}

} // namespace emu
