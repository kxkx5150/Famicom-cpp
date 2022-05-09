#include "mem.h"
#include <cstdio>

Mem::Mem(Mapper0 *_mapper, Dma *_dma, Io *_io, APU *_apu)
{
    mapper = _mapper;
    dma    = _dma;
    io     = _io;
    apu    = _apu;
}
Mem::~Mem()
{
}
void Mem::init()
{
    printf("mem init\n");
    reset();
    mapper->init();
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
            switch (addr) {
                case 0x4016: {
                    uint8_t ret = io->get_latched_ctrl_state(1) & 1;
                    io->set_latched_ctrl_state(1);
                    return ret | 0x40;
                }
                case 0x4017: {
                    uint8_t ret = io->get_latched_ctrl_state(2) & 1;
                    io->set_latched_ctrl_state(2);
                    return ret | 0x40;
                }
            }

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
            break;
        }
        case 0x2000: {
            switch (addr & 0x0007) {
                case 0x00: {
                    mapper->ppu->write_ppu_ctrl0_reg(data);
                    break;
                }
                case 0x01: {
                    mapper->ppu->write_ppu_ctrl1_reg(data);
                    break;
                }
                case 0x03: {
                    mapper->ppu->write_sprite_addr_reg(data);
                    break;
                }
                case 0x04: {
                    mapper->ppu->write_sprite_data(data);
                    break;
                }
                case 0x05: {
                    mapper->ppu->write_scroll_reg(data);
                    break;
                }
                case 0x06: {
                    mapper->ppu->write_ppu_addr_reg(data);
                    break;
                }
                case 0x07: {
                    mapper->ppu->write_ppu_data_reg(data);
                    break;
                }
            }
            break;
        }
        case 0x4000: {
            switch (addr) {

                case 0x4000:
                case 0x4001:
                case 0x4002:
                case 0x4003:
                case 0x4004:
                case 0x4005:
                case 0x4006:
                case 0x4007:
                case 0x4008:
                case 0x4009:
                case 0x4010:
                case 0x400a:
                case 0x400b:
                case 0x400c:
                case 0x400d:
                case 0x400e:
                case 0x400f:
                case 0x4011:
                case 0x4012:
                case 0x4013:{
                    apu->write(addr, data);
                    break;
                }

                case 0x4014: {
                    dma->run(data, mapper->ppu, ram);
                    break;
                }
                case 0x4015:{
                    apu->write(addr, data);
                    break;
                }
                case 0x4016: {
                    if ((data & 0x01) > 0) {
                        io->set_ctrllatched(true);
                    } else {
                        io->set_ctrllatched(false);
                    }
                    break;
                }
                case 0x4017:{
                    apu->write(addr, data);
                    break;
                }




            }
            break;
        }
        case 0x6000: {
            break;
        }
        case 0x8000: {
            mapper->write(addr, data);
            break;
        }
        case 0xa000: {
            mapper->write(addr, data);
            break;
        }
        case 0xc000: {
            mapper->write(addr, data);
            break;
        }
        case 0xe000: {
            mapper->write(addr, data);
            break;
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
    for (size_t i = 0; i < 0x800; i++) {
        ram[i] = 0;
    }
}
