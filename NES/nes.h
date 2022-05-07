#define SDL_MAIN_HANDLED

#include "cpu.h"
#include "dma.h"
#include "irq.h"
#include "mapper0.h"
#include "mem.h"
#include "ppu.h"
#include "rom.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


class Nes {
  public:
    Irq     *irq;
    Rom     *rom;
    Ppu     *ppu;
    Mapper0 *mapper;
    Dma     *dma;
    Mem     *mem;
    Cpu     *cpu;

  public:
    Nes();
    ~Nes();

    void init();
    void set_rom();
    void start(bool cputest = false);
    void main_loop(size_t count, bool cputest);
    void UpdateTexture(SDL_Texture *texture, uint32_t *imgdata);
};
