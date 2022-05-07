#ifndef _H_HEAD1_
#define _H_HEAD1_

#include "dma.h"
#include "mapper0.h"


class Mem {
  public:
    uint8_t  ram[0x800]{};
    Mapper0 *mapper = nullptr;
    Dma     *dma    = nullptr;

  public:
    Mem(Mapper0 *_mapper, Dma *dma);
    ~Mem();
    void     init();
    uint8_t  get(uint16_t addr);
    void     set(uint16_t addr, uint8_t data);
    uint16_t get16(uint16_t addr);

    void reset();
};

#endif    // _H_HEAD1_
