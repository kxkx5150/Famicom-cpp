#include "ppu.h"
#include <cstdio>

Ppu::Ppu(Rom *_rom, Irq *_irq)
{
    rom = _rom;
    irq = _irq;
}
Ppu::~Ppu()
{
}
void Ppu::init()
{
    reset();
}
void Ppu::start()
{
    printf("ppu start\n");
    crate_spbit_array();
    screen_mirroring = rom->screen_mirroring;
    if (screen_mirroring == VERTICAL) {
        set_mode_mirror(false);
    } else if (screen_mirroring == HORIZONTAL) {
        set_mode_mirror(true);
    } else {
        init_mirrors(0, 1, 2, 3);
    }

    ppux        = 341;
    line        = 0;
    sprite_zero = false;
    imgok       = false;
}
void Ppu::reset()
{
    imgidx           = 0;
    scroll_reg_flg   = false;
    ppu_addr_reg_flg = false;
    ppu_addr_buffer  = 0;
    ppu_read_buffer  = 0;
    ppu_addr         = 0;
    h_scroll_val     = 0;
    ppux             = 341;
    line             = 0;
    sprite_zero      = false;
    imgok            = false;
    clear_arryas();
}
void Ppu::crate_spbit_array()
{
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 256; j++) {
            for (int k = 0; k < 8; k++) {
                uint64_t lval          = (((i << k) & 0x80) >> 7);
                uint64_t rval          = (((j << k) & 0x80) >> 6);
                uint64_t val           = (lval | rval);
                spbit_pattern[i][j][k] = val;
            }
        }
    }
}
void Ppu::clear_arryas()
{
}
void Ppu::set_mode_mirror(bool value)
{
    if (value) {
        init_mirrors(0, 0, 1, 1);
    } else {
        init_mirrors(0, 1, 0, 1);
    }
}
void Ppu::init_mirrors(uint64_t value0, uint64_t value1, uint64_t value2, uint64_t value3)
{
    set_chr_rom_data1k(8, value0 + 8 + 0x0100);
    set_chr_rom_data1k(9, value1 + 8 + 0x0100);
    set_chr_rom_data1k(10, value2 + 8 + 0x0100);
    set_chr_rom_data1k(11, value3 + 8 + 0x0100);
}
void Ppu::set_chr_rom_data1k(uint64_t page, uint64_t romPage)
{
    if (0x0100 <= romPage) {
        rom->chrrom_state[page] = romPage;
        auto prom               = vrams[romPage & 0xff];
        for (uint64_t i = 0; i < 1024; i++) {
            vram[page][i] = prom[i];
        }
    } else if (0 < rom->chr_rom_page_count) {
        uint64_t tmp            = romPage % (rom->chr_rom_page_count * 8);
        rom->chrrom_state[page] = tmp;

        auto prom = rom->chrrom_pages[rom->chrrom_state[page]];
        auto len  = rom->chrrom_page_len[rom->chrrom_state[page]];

        for (uint64_t i = 0; i < len; i++) {
            vram[page][i] = prom[i];
        }
    }
}
void Ppu::set_chrrom_pages1k(uint64_t rompage0, uint64_t rompage1, uint64_t rompage2, uint64_t rompage3,
                             uint64_t rompage4, uint64_t rompage5, uint64_t rompage6, uint64_t rompage7)
{
    set_chr_rom_data1k(0, rompage0);
    set_chr_rom_data1k(1, rompage1);
    set_chr_rom_data1k(2, rompage2);
    set_chr_rom_data1k(3, rompage3);
    set_chr_rom_data1k(4, rompage4);
    set_chr_rom_data1k(5, rompage5);
    set_chr_rom_data1k(6, rompage6);
    set_chr_rom_data1k(7, rompage7);
}
void Ppu::set_chr_rom_page(uint64_t num)
{
    num <<= 3;
    for (uint64_t i = 0; i < 8; i++) {
        set_chr_rom_data1k(i, num + i);
    }
}
void Ppu::run(uint64_t cpuclock)
{
    uint64_t tmpx = ppux;
    ppux += cpuclock * 3;

    while (341 <= ppux) {
        ppux -= 341;
        line += 1;
        tmpx        = 0;
        sprite_zero = false;

        if (line < 240) {
            render_frame();
        } else if (line == 240) {
            in_vblank();
            continue;
        } else if (line == 262) {
            post_render();
        }
    }
    if (sprite_zero && (regs[0x02] & 0x40) != 0x40) {
        uint64_t i = ppux > 255 ? 255 : ppux;
        while (tmpx <= i) {
            if (sp_line_buffer[tmpx] == 0) {
                regs[0x02] |= 0x40;
                break;
            }
            tmpx += 1;
        }
    }
}
void Ppu::render_frame()
{
    if (is_screen_enable() || is_sprite_enable()) {
        ppu_addr = (ppu_addr & 0xfbe0) | (ppu_addr_buffer & 0x041f);

        if (8 <= line && line < 232) {
            build_bg();
            build_sp_line();
            for (uint64_t p = 0; p < 256; p++) {
                int  idx = palette[bg_line_buffer[p]];
                rgb *pal = &PALLETE_TABLE[idx];
                set_img_data(pal->r, pal->g, pal->b);
            }
        } else {
            for (uint64_t p = 0; p < 264; p++) {
                bg_line_buffer[p] = 0x10;
            }
            build_sp_line();
        }

        if ((ppu_addr & 0x7000) == 0x7000) {
            ppu_addr &= 0x8fff;

            if ((ppu_addr & 0x03e0) == 0x03a0) {
                ppu_addr = (ppu_addr ^ 0x0800) & 0xfc1f;
            } else if ((ppu_addr & 0x03e0) == 0x03e0) {
                ppu_addr &= 0xfc1f;
            } else {
                ppu_addr += 0x0020;
            }
        } else {
            ppu_addr += 0x1000;
        }
    } else if (8 <= line && line < 232) {
        for (uint64_t p = 0; p < 256; p++) {
            rgb *pal = &PALLETE_TABLE[palette[0x10]];
            set_img_data(pal->r, pal->g, pal->b);
        }
    }
}
void Ppu::build_bg()
{
    if ((regs[0x01] & 0x08) != 0x08) {
        for (uint64_t p = 0; p < 264; p++) {
            bg_line_buffer[p] = 0x10;
        }
        return;
    }
    build_bg_line();
    if ((regs[0x01] & 0x02) != 0x02) {
        for (uint64_t x = 0; x < 8; x++) {
            bg_line_buffer[x] = 0x10;
        }
    }
}
void Ppu::build_bg_line()
{
    uint64_t nameaddr       = 0x2000 | (ppu_addr & 0x0fff);
    uint64_t tableaddr      = ((ppu_addr & 0x7000) >> 12) | ((regs[0x00] & 0x10) << 8);
    uint64_t name_addr_h    = nameaddr >> 10;
    uint64_t name_addr_l    = nameaddr & 0x03ff;
    uint64_t pre_name_addrh = name_addr_h;
    uint64_t s              = h_scroll_val;
    uint64_t q              = 0;

    for (uint64_t p = 0; p < 33; p++) {
        uint8_t *vrm     = vram[pre_name_addrh];
        uint64_t ptndist = ((vrm[name_addr_l]) << 4) | tableaddr;
        uint8_t *vvrm    = vram[ptndist >> 10];
        ptndist &= 0x03ff;

        uint64_t lval = (name_addr_l & 0x0380) >> 4;
        uint64_t rval = ((name_addr_l & 0x001c) >> 2) + 0x03c0;

        uint64_t lval2 = (name_addr_l & 0x0040) >> 4;
        uint64_t rval2 = name_addr_l & 0x0002;
        uint64_t attr  = ((vrm[lval | rval] << 2) >> (lval2 | rval2)) & 0x0c;

        uint64_t spbidx1 = vvrm[ptndist];
        uint64_t spbidx2 = vvrm[(ptndist + 8)];
        uint8_t *ptn     = spbit_pattern[spbidx1][spbidx2];

        while (s < 8) {
            uint64_t idx      = ptn[s] | attr;
            bg_line_buffer[q] = PALLETE[idx];
            q += 1;
            s += 1;
        }
        s = 0;

        if ((name_addr_l & 0x001f) == 0x001f) {
            name_addr_l &= 0xffe0;
            name_addr_h ^= 0x01;
            pre_name_addrh = name_addr_h;
        } else {
            name_addr_l += 1;
        }
    }
}
void Ppu::build_sp_line()
{
    uint64_t spclip = (regs[0x01] & 0x04) == 0x04 ? 0 : 8;

    if ((regs[0x01] & 0x10) == 0x10) {

        for (uint64_t p = 0; p < 264; p++) {
            sp_line_buffer[p] = 256;
        }

        uint64_t spptableaddr = ((regs[0x00] & 0x08)) << 9;
        uint64_t count        = 0;
        uint64_t bzsize       = is_bigsize();

        for (uint64_t i = 0; i < 252; i += 4) {
            uint64_t isy = (sprite_ram[i] + 1);
            if (isy > line || (isy + bzsize <= line)) {
                continue;
            }

            if (i == 0) {
                sprite_zero = true;
            }

            count += 1;
            if (count == 9) {
                break;
            }

            uint64_t attr      = sprite_ram[i + 2];
            uint64_t attribute = (((attr & 0x03)) << 2) | 0x10;
            uint64_t bgsp      = (attr & 0x20) == 0x00;

            uint64_t x  = (sprite_ram[i + 3]);
            uint64_t ex = x + 8;
            if (ex > 256) {
                ex = 256;
            }

            uint64_t iy   = (attr & 0x80) == 0x80 ? (bzsize - 1 - (line - isy)) : (line - isy);
            uint64_t lval = ((sprite_ram[i + 1]) << 4) + spptableaddr;
            uint64_t rval = ((sprite_ram[i + 1] & 0xfe) << 4) + ((sprite_ram[i + 1] & 0x01) << 12);
            uint64_t sval = bzsize == 8 ? lval : rval;

            uint64_t tilenum = ((iy & 0x08) << 1) + (iy & 0x07) + sval;
            uint64_t tlow    = tilenum & 0x03ff;

            uint64_t is = 7;
            uint64_t ia = -1;
            if ((attr & 0x40) == 0x00) {
                is = 0;
                ia = 1;
            }

            uint64_t ptnidxl = vram[tilenum >> 10][tlow];
            uint64_t ptnidxr = vram[tilenum >> 10][tlow + 8];
            uint8_t *ptn     = spbit_pattern[ptnidxl][ptnidxr];

            while (x < ex) {

                uint64_t tptn = ptn[is];
                if (tptn != 0x00 && (sp_line_buffer[x] == 256)) {
                    sp_line_buffer[x] = i;
                    if (x >= spclip && (bgsp || bg_line_buffer[x] == 0x10)) {
                        bg_line_buffer[x] = tptn | attribute;
                    }
                }
                x += 1;
                is += ia;
            }
        }

        if (8 <= count) {
            regs[0x02] |= 0x20;
        } else {
            regs[0x02] &= 0xdf;
        }
    }
}
void Ppu::in_vblank()
{
    scroll_reg_flg = false;
    regs[0x02] &= 0x1f;
    regs[0x02] |= 0x80;
    if ((regs[0x00] & 0x80) == 0x80) {
        irq->set_nmi(true);
    }
}
void Ppu::post_render()
{
    line = 0;
    if (is_screen_enable() || is_sprite_enable()) {
        ppu_addr = ppu_addr_buffer;
    }
    regs[0x02] &= 0x7f;
    imgok = true;
}
void Ppu::set_img_data(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t dots   = (0xFF000000 | (r << 16) | (g << 8) | b);
    imgdata[imgidx] = dots;
    imgidx++;
}
void Ppu::clear_img()
{
    imgidx = 0;
    imgok  = false;
}
bool Ppu::get_img_status()
{
    if (imgok) {
        return true;
    } else {
        return false;
    }
}
uint32_t *Ppu::get_img_data()
{
    return imgdata;
}

bool Ppu::is_screen_enable()
{
    return (regs[0x01] & 0x08) == 0x08;
}
bool Ppu::is_sprite_enable()
{
    return (regs[0x01] & 0x10) == 0x10;
}
uint8_t Ppu::is_bigsize()
{
    return (regs[0x00] & 0x20) == 0x20 ? 16 : 8;
}
