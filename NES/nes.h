#include "cpu.h"
#include "irq.h"
#include "mapper0.h"
#include "mem.h"
#include "ppu.h"
#include "rom.h"


class Nes {
  public:
    Irq     *irq;
    Rom     *rom;
    Ppu     *ppu;
    Mapper0 *mapper;
    Mem     *mem;
    Cpu     *cpu;

  public:
    Nes();
    ~Nes();

    void set_rom();
    void start(bool cputest = false);
    void main_loop(size_t count, bool cputest);
};
