# Chip8 emulator

## Info

Simple Chip8 emulator to learn the basics of emulation.

## Dependencies

### SDL2 (graphics)

Debian based (including Ubuntu):

```bash
# library
sudo apt install libsdl2
# everything necessary to build programs that use SDL
sudo apt install libsdl2-dev
```

Red Hat based (including Fedora):

```bash
# library
sudo dnf install SDL2
# everything necessary to build programs that use SDL
sudo dnf install SDL2-devel
```

Arch:

```bash
# everything
sudo pacman -S sdl2
```

Gentoo:

```bash
# everything
sudo emerge libsdl2
```

Other:

[SDL2 Installation guide](https://wiki.libsdl.org/SDL2/Installation)

## How to run

Compile:

```bash
make
```

And run:

```bash
./bin/emu SCALE ROM
```

```bash
# example
./bin/emu 10 ./roms/pong.ch8
```
