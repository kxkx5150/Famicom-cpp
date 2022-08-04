#ifndef _H_MAPPER0
#define _H_MAPPER0

#include "mapper.h"
#include "ppu.h"
#include "rom.h"



class Mapper0 : public MapperBase {
  public:
    Rom *rom = nullptr;
    Ppu *ppu = nullptr;

  public:
    Mapper0(Rom *_rom, Ppu *_ppu);
    ~Mapper0();

    void init();
    void set_rom(std::string filename);
};

#endif    // _H_MAPPER0
