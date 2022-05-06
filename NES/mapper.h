#ifndef _H_MAPPER_
#define _H_MAPPER_

#include <fstream>

using namespace std;

class MapperBase {
  public:
    uint8_t mapper_reg[0x800]{};

  public:
    void init()
    {
    }
    uint8_t read_low(uint16_t addr)
    {
        return 0x00;
    }
    void write_low(uint16_t addr, uint8_t data)
    {
    }
    uint8_t read_ppudata()
    {
        return 0x00;
    }
    void write_ppudata()
    {
    }
    void build_bgline()
    {
    }
    void build_spriteline()
    {
    }
    uint8_t read_sram()
    {
        return 0x00;
    }
    void write_sram()
    {
    }
    void write(uint16_t addr, uint8_t data)
    {
    }
    void hsync()
    {
    }
    void cpusync()
    {
    }
    void setirq()
    {
    }
    void clearirq()
    {
    }
    uint8_t out_exsound()
    {
        return 0x00;
    }
    void exsound_sync()
    {
    }
    void getstate()
    {
    }
    void setstate()
    {
    }
};

#endif    // _H_MAPPER_
