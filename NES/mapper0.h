#ifndef _H_MAPPER0
#define _H_MAPPER0

#include "mapper.h"
#include "rom.h"
#include "ppu.h"

using namespace std;

class Mapper0 : public MapperBase {
  public:
    Rom *rom = nullptr;
    Ppu *ppu = nullptr;

  public:
    Mapper0(Rom *_rom, Ppu *_ppu);
    ~Mapper0();

    void init();
    void set_rom(string filename);
};

#endif    // _H_MAPPER0
