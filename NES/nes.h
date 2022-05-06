#include "cpu.h"
#include "mapper0.h"
#include "mem.h"
#include "rom.h"

class Nes {
  public:
    Rom     *rom;
    Mapper0 *mapper;
    Mem     *mem;
    Cpu     *cpu;

  public:
    Nes();
    ~Nes();

    void set_rom();
    void start(bool cputest);
    void main_loop(size_t count, bool cputest);
};
