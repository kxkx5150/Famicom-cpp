#ifndef _H_ROM_
#define _H_ROM_

#include <cstdint>
#include <vector>
#include <string>

enum Mirroring
{
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN
};

class Rom {
  public:
    uint64_t prg_rom_page_count;
    uint64_t chr_rom_page_count;
    int    screen_mirroring = HORIZONTAL;
    bool   sram_enable;
    bool   trainer_Enable;
    bool   four_screen;
    uint64_t mapper_number;

    uint8_t *rom;
    uint64_t   romlen;
    uint8_t *roms[16]{};
    uint8_t *prgrom_pages[16]{};
    uint8_t *chrrom_pages[16]{};

    uint64_t prgrom_page_len[16]{};
    uint64_t chrrom_page_len[16]{};

    int8_t prgrom_state[4]{};
    int8_t chrrom_state[16]{};

    std::vector<uint8_t> srams;

  public:
    Rom();
    ~Rom();

    void set_rom(std::string filename);
    void create_pages();
    void set_prgrom_page_8k(int page, int rompage);
    void set_prgrom_page(int no, int num);

};

#endif    // _H_ROM_
