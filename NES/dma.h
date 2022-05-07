#ifndef _H_DMA
#define _H_DMA

#include "ppu.h"

class Dma {
    bool on = false;

  public:
    void run(uint8_t data, Ppu *ppu, uint8_t *ram){
        size_t offset = data << 8;
        for (size_t i = 0; i < 0x100; i++){
            ppu->sprite_ram[i] = ram[offset];
            offset += 1;
        }
        on = true;
    }
    bool get_status(){
        return on;
    }
    void clear(){
        on = false;
    }
};


#endif
