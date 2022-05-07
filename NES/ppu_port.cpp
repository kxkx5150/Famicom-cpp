#include "ppu.h"
#include <cstdint>

void Ppu::write_scroll_reg(uint8_t value)
{
    uint32_t val = value;
    regs[0x05]   = value;
    if (scroll_reg_flg) {
        ppu_addr_buffer = (ppu_addr_buffer & 0x8c1f) | ((val & 0xf8) << 2) | ((val & 0x07) << 12);
    } else {
        ppu_addr_buffer = (ppu_addr_buffer & 0xffe0) | ((val & 0xf8) >> 3);
        h_scroll_val    = val & 7;
    }
    scroll_reg_flg = !scroll_reg_flg;
}
void Ppu::write_ppu_ctrl0_reg(uint8_t value)
{
    uint32_t val    = value;
    regs[0x00]      = value;
    ppu_addr_buffer = (ppu_addr_buffer & 0xf3ff) | ((val & 0x03) << 10);
}
void Ppu::write_ppu_ctrl1_reg(uint8_t value)
{
    regs[0x01] = value;
}
uint8_t Ppu::read_ppu_status_reg()
{
    auto result = regs[0x02];
    regs[0x02] &= 0x1f;
    scroll_reg_flg   = false;
    ppu_addr_reg_flg = false;
    return result;
}
void Ppu::write_ppu_addr_reg(uint8_t value)
{
    uint32_t val = value;
    regs[0x06]   = value;
    if (ppu_addr_reg_flg) {
        ppu_addr_buffer = (ppu_addr_buffer & 0xff00) | val;
        ppu_addr        = ppu_addr_buffer;
    } else {
        ppu_addr_buffer = (ppu_addr_buffer & 0x00ff) | ((val & 0x3f) << 8);
    }
    ppu_addr_reg_flg = !ppu_addr_reg_flg;
}
uint8_t Ppu::read_ppu_data_reg()
{
    auto     tmp    = ppu_read_buffer;
    uint32_t addr   = ppu_addr & 0x3fff;
    auto     aryidx = addr >> 10;
    auto     idx    = addr & 0x03ff;

    ppu_read_buffer = vram[addr >> 10][addr & 0x03ff];

    uint32_t val = (regs[0x00] & 0x04) == 0x04 ? 32 : 1;
    ppu_addr     = (ppu_addr + val) & 0xffff;
    return tmp;
}
void Ppu::write_ppu_data_reg(uint8_t value)
{
    regs[0x07]                                    = value;
    uint32_t tmpppu_addr                          = ppu_addr & 0x3fff;
    vram[tmpppu_addr >> 10][tmpppu_addr & 0x03ff] = value;

    if (tmpppu_addr < 0x3000) {
        uint32_t val = (regs[0x00] & 0x04) == 0x04 ? 32 : 1;
        ppu_addr     = (ppu_addr + val) & 0xffff;
        return;
    }

    if (tmpppu_addr < 0x3eff) {
        vram[(tmpppu_addr - 0x1000) >> 10][(tmpppu_addr - 0x1000) & 0x03ff] = value;
        uint32_t val                                                        = (regs[0x00] & 0x04) == 0x04 ? 32 : 1;
        ppu_addr                                                            = (ppu_addr + val) & 0xffff;
        return;
    }

    uint32_t palNo = tmpppu_addr & 0x001f;
    if (palNo == 0x00 || palNo == 0x10) {
        palette[0x10] = (value & 0x3f);
        palette[0x00] = palette[0x10];
    } else {
        palette[palNo] = value & 0x3f;
    }
    uint32_t val = (regs[0x00] & 0x04) == 0x04 ? 32 : 1;
    ppu_addr     = (ppu_addr + val) & 0xffff;
}
void Ppu::write_sprite_data(uint8_t value)
{
    uint32_t idx    = regs[0x03];
    sprite_ram[idx] = value;
    regs[0x03]      = (regs[0x03] + 1) & 0xff;
}
void Ppu::write_sprite_addr_reg(uint8_t value)
{
    regs[0x03] = value;
}
