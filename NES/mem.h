#ifndef _H_HEAD1_
#define _H_HEAD1_

#include "dma.h"
#include "io.h"
#include "mapper0.h"
#include "apu/Simple_Apu.h"


class Mem {
  public:
    uint8_t     ram[0x800]{};
    Mapper0    *mapper = nullptr;
    Dma        *dma    = nullptr;
    Io         *io     = nullptr;
    Simple_Apu *apu    = nullptr;

  public:
    Mem(Mapper0 *_mapper, Dma *dma, Io *_io, Simple_Apu *_apu);
    ~Mem();
    void     init();
    uint8_t  get(uint16_t addr);
    void     set(uint16_t addr, uint8_t data);
    uint16_t get16(uint16_t addr);

    void reset();
};

#endif
