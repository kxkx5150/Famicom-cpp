#define SDL_MAIN_HANDLED

#include "apu/Simple_Apu.h"
#include "apu/Sound_Queue.h"
#include "cpu.h"
#include "dma.h"
#include "io.h"
#include "irq.h"
#include "mapper0.h"
#include "mem.h"
#include "ppu.h"
#include "rom.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

class Nes {

  public:
    Io          *io;
    Irq         *irq;
    Rom         *rom;
    Ppu         *ppu;
    Mapper0     *mapper;
    Dma         *dma;
    Mem         *mem;
    Cpu         *cpu;
    Simple_Apu  *apu         = nullptr;
    Sound_Queue *sound_queue = nullptr;

  public:
    Nes();
    ~Nes();

    void    init();
    void    set_rom();
    void    start(bool cputest = false);
    void    main_loop(uint64_t count, bool cputest);
    uint8_t keycode_to_pad(SDL_Event event);
    void    UpdateTexture(SDL_Texture *texture, uint32_t *imgdata);
};
