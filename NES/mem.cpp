#include "mem.h"

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

Mem::Mem(Mapper0 *_mapper)
{
    mapper = _mapper;
}
Mem::~Mem()
{
}
void Mem::init()
{
}
uint8_t Mem::get(uint16_t addr)
{
    switch (addr & 0xe000) {
        case 0x0000: {
            return ram[addr & 0x7ff];
        }
        case 0x2000: {
            switch (addr & 0x0007) {
                case 0x0002: {
                    return mapper->ppu->read_ppu_status_reg();
                }
                case 0x0007: {
                    return mapper->ppu->read_ppu_data_reg();
                }
            }
            return 0;
        }
        case 0x4000: {
            return 0;
        }
        case 0x6000: {
            return 0;
        }
        case 0x8000: {
            return mapper->rom->roms[0][(addr & 0x1fff)];
        }
        case 0xa000: {
            return mapper->rom->roms[1][(addr & 0x1fff)];
        }
        case 0xc000: {
            return mapper->rom->roms[2][(addr & 0x1fff)];
        }
        case 0xe000: {
            return mapper->rom->roms[3][(addr & 0x1fff)];
        }
    }
    return 0;
}
void Mem::set(uint16_t addr, uint8_t data)
{
    switch (addr & 0xe000) {
        case 0x0000: {
            ram[addr & 0x7ff] = data;
        }
        case 0x2000: {
            switch (addr & 0x0007) {
                case 0x00: {
                    mapper->ppu->write_ppu_ctrl0_reg(data);
                }
                case 0x01: {
                    mapper->ppu->write_ppu_ctrl1_reg(data);
                }
                case 0x03: {
                    mapper->ppu->write_sprite_addr_reg(data);
                }
                case 0x04: {
                    mapper->ppu->write_sprite_data(data);
                }
                case 0x05: {
                    mapper->ppu->write_scroll_reg(data);
                }
                case 0x06: {
                    mapper->ppu->write_ppu_addr_reg(data);
                }
                case 0x07: {
                    mapper->ppu->write_ppu_data_reg(data);
                }
            }
        }
        case 0x4000: {
        }
        case 0x6000: {
        }
        case 0x8000: {
            mapper->write(addr, data);
        }
        case 0xa000: {
            mapper->write(addr, data);
        }
        case 0xc000: {
            mapper->write(addr, data);
        }
        case 0xe000: {
            mapper->write(addr, data);
        }
    }
}
uint16_t Mem::get16(uint16_t addr)
{
    uint16_t l = get(addr);
    uint16_t h = get(addr + 1);
    uint16_t r = l | (h << 8);
    return r;
}
void Mem::reset()
{
    for (size_t i = 0; i < SIZE_OF_ARRAY(ram); i++) {
        ram[i] = 0;
    }
}
