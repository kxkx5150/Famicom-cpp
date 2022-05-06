#include "rom.h"

#define SIZE_OF_ARRAY(array) (sizeof(array) / sizeof(array[0]))

Rom::Rom()
{
    for (int i = 0; i < 16; i++) {
        roms[i] = new uint8_t[0x2000]{};
    }
}
Rom::~Rom()
{
    delete[] rom;

    for (int i = 0; i < 16; i++) {
        delete[] roms[i];
        delete [] prgrom_pages[i];
        delete [] chrrom_pages[i];
    }
}
void Rom::set_rom(string filename)
{
    FILE  *f        = fopen(filename.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    const int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    romlen = size;
    rom    = new uint8_t[size];
    fread(rom, size, 1, f);
    fclose(f);

    if (!(rom[0] == 0x4e && rom[1] == 0x45 && rom[2] == 0x53 && rom[3] == 0x1a)) {
        return;
    }

    prg_rom_page_count = rom[4];
    chr_rom_page_count = rom[5];

    four_screen             = (rom[6] & 0b1000) != 0;
    bool vertical_mirroring = (rom[6] & 0b1) != 0;
    if (four_screen) {
        screen_mirroring = FOUR_SCREEN;
    } else if (vertical_mirroring) {
        screen_mirroring = VERTICAL;
    } else {
        screen_mirroring = HORIZONTAL;
    }

    sram_enable    = (rom[6] & 0x02) != 0;
    trainer_Enable = (rom[6] & 0x04) != 0;
    mapper_number  = (rom[6] >> 4) | (rom[7] & 0xf0);

    create_pages();
}
void Rom::create_pages()
{
    int hlen      = 0x0010;
    int prg_psize = 0x4000;
    int chr_psize = 0x2000;

    if (prg_rom_page_count > 0) {
        for (int i = 0; i < (prg_rom_page_count * 2); i++) {
            int stat = hlen + (prg_psize / 2) * i;
            int end  = stat + prg_psize / 2;

            const int size     = end - stat;
            auto     *prom     = new uint8_t[size];
            prgrom_page_len[i] = size;

            for (int j = 0; j < size; j++) {

                prom[j] = rom[stat + j];
            }
            prgrom_pages[i] = prom;
        }
    }

    if (chr_rom_page_count > 0) {
        for (int i = 0; i < (chr_rom_page_count * 8); i++) {
            int stat = hlen + prg_psize * prg_rom_page_count + (chr_psize / 8) * i;
            int end  = stat + chr_psize / 2;
            if (end > romlen) {
                end = romlen;
            }
            const int size     = end - stat;
            auto     *crom     = new uint8_t[size];
            chrrom_page_len[i] = size;

            for (int j = 0; j < size; j++) {
                crom[j] = rom[stat + j];
            }
            chrrom_pages[i] = crom;
        }
    }
}
void Rom::clear_roms()
{
}
void Rom::set_prgrom_page_8k(int page, int rompage)
{
    if (rompage < 0) {
        prgrom_state[page] = rompage;

        for (int i = 0; i < 0x2000; i++) {
            roms[page][i] = 0;
        }
    } else {
        prgrom_state[page] = rompage % (prg_rom_page_count * 2);

        size_t idx    = prgrom_state[page];
        auto   prgrom = prgrom_pages[idx];
        auto   plen   = prgrom_page_len[idx];

        for (int i = 0; i < plen; i++) {
            roms[page][i] = prgrom[i];
        }
    }
}
void Rom::set_prgrom_page(int no, int num)
{
    set_prgrom_page_8k((no * 2), (num * 2));
    set_prgrom_page_8k((no * 2 + 1), (num * 2 + 1));
}
