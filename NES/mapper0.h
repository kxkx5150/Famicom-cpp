#ifndef _H_MAPPER0
#define _H_MAPPER0

#include "mapper.h"
#include "rom.h"

using namespace std;

class Mapper0 : public MapperBase {
  public:
    Rom *rom = nullptr;

  public:
    Mapper0(Rom *_rom);
    ~Mapper0();

    void set_rom();
};

#endif    // _H_MAPPER0
