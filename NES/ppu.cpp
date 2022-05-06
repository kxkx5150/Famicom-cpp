#include "ppu.h"
#include <corecrt.h>


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
    printf("ppu start");
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
                size_t lval            = (((i << k) & 0x80) >> 7);
                size_t rval            = (((j << k) & 0x80) >> 6);
                size_t val             = (lval | rval);
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
void Ppu::init_mirrors(size_t value0, size_t value1, size_t value2, size_t value3)
{
    set_chr_rom_data1k(8, value0 + 8 + 0x0100);
    set_chr_rom_data1k(9, value1 + 8 + 0x0100);
    set_chr_rom_data1k(10, value2 + 8 + 0x0100);
    set_chr_rom_data1k(11, value3 + 8 + 0x0100);
}
void Ppu::set_chr_rom_data1k(size_t page, size_t romPage)
{
    if (0x0100 <= romPage) {
        rom->chrrom_state[page] = romPage;

        for (size_t i = 0; i < 1024; i++) {
            vram[page][i] = vrams[romPage][i];
        }
    } else if (0 < rom->chr_rom_page_count) {
        size_t tmp              = romPage % (rom->chr_rom_page_count * 8);
        rom->chrrom_state[page] = tmp;

        auto prom = rom->chrrom_pages[rom->chrrom_state[page]];
        auto len  = rom->prgrom_page_len[rom->chrrom_state[page]];

        for (size_t i = 0; i < len; i++) {
            vram[page][i] = prom[i];
        }
    }
}
void Ppu::set_chrrom_pages1k(size_t rompage0, size_t rompage1, size_t rompage2, size_t rompage3, size_t rompage4,
                             size_t rompage5, size_t rompage6, size_t rompage7)
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
void Ppu::set_chr_rom_page(size_t num)
{
    num <<= 3;
    for (size_t i = 0; i < 8; i++) {
        set_chr_rom_data1k(i, num + i);
    }
}
void Ppu::run(size_t cpuclock)
{
    size_t tmpx = ppux;
    ppux += cpuclock * 3;

    while (341 <= ppux) {
        ppux -= 341;
        line += 1;
        tmpx = 0;
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
    // if (sprite_zero && (regs[0x02] & 0x40) != 0x40) {
    //     let i = if ppux > 255 { 255 } else { ppux };
    //     while tmpx <= i {
    //         if (sp_line_buffer[tmpx] == 0) {
    //             regs[0x02] |= 0x40;
    //             break;
    //         }
    //         tmpx += 1;
    //     }
    // }
}
void Ppu::render_frame()
{
    if (is_screen_enable() || is_sprite_enable()) {
        ppu_addr = (ppu_addr & 0xfbe0) | (ppu_addr_buffer & 0x041f);

        if (8 <= line && line < 232) {
            build_bg();
            // build_sp_line();
            for (size_t p = 0; p < 256; p++){
                auto idx = palette[bg_line_buffer[p]];
                auto pal = PALLETE_TABLE[idx];
                set_img_data(pal);
            }
        } else {
            for (size_t p = 0; p < 264; p++){
                bg_line_buffer[p] = 0x10;
            }
            // build_sp_line();
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
        for (size_t p = 0; p < 256; p++){
            auto pal = PALLETE_TABLE[palette[0x10]];
            set_img_data(pal);
        }
    }
}
void Ppu::build_bg()
{
    if ((regs[0x01] & 0x08) != 0x08) {
        for (size_t p = 0; p < 264; p++){
            bg_line_buffer[p] = 0x10;
        }
        return;
    }
    build_bg_line();
    if ((regs[0x01] & 0x02) != 0x02) {
        for (size_t x = 0; x < 8; x++){
            bg_line_buffer[x] = 0x10;
        }
    }
}
void Ppu::build_bg_line()
{
    size_t nameaddr = 0x2000 | (ppu_addr & 0x0fff);
    size_t tableaddr =
        ((ppu_addr & 0x7000) >> 12) | ((regs[0x00] & 0x10) << 8);
    size_t name_addr_h = nameaddr >> 10;
    size_t name_addr_l = nameaddr & 0x03ff;
    size_t pre_name_addrh = name_addr_h;
    size_t s = h_scroll_val;
    size_t q = 0;

    for (size_t p = 0; p < 33; p++){
        auto vrm = vram[pre_name_addrh];
        size_t ptndist = ((vrm[name_addr_l]) << 4) | tableaddr;
        auto vvrm = vram[ptndist >> 10];
        ptndist &= 0x03ff;

        size_t lval = (name_addr_l & 0x0380) >> 4;
        size_t rval = ((name_addr_l & 0x001c) >> 2) + 0x03c0;

        size_t lval2 = (name_addr_l & 0x0040) >> 4;
        size_t rval2 = name_addr_l & 0x0002;
        size_t attr = ((vrm[lval | rval] << 2) >> (lval2 | rval2)) & 0x0c;

        size_t spbidx1 = vvrm[ptndist];
        size_t spbidx2 = vvrm[(ptndist + 8)];
        auto ptn = spbit_pattern[spbidx1][spbidx2];

        while (s < 8) {
            size_t idx = ptn[s] | attr;
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
void Ppu::set_img_data(std::vector<uint8_t> rgb)
{
    uint32_t dots = (0xFF000000 | (rgb[0] << 16) | (rgb[1] << 8) | rgb[2]);
    imgdata[imgidx] = dots;
    imgidx++;
}
void Ppu::clear_img()
{
    imgidx = 0;
    imgok = false;
}
bool Ppu::get_img_status()
{
    if (imgok) {
        return true;
    } else {
        return false;
    }
}
uint32_t* Ppu::get_img_data()
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









