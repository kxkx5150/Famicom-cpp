#ifndef _H_ROM_
#define _H_ROM_

#include <fstream>
#include <vector>

using namespace std;

enum mirroring
{
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN
};

class Rom {
  public:
    size_t prg_rom_page_count;
    size_t chr_rom_page_count;
    int    screen_mirroring = HORIZONTAL;
    bool   sram_enable;
    bool   trainer_Enable;
    bool   four_screen;
    size_t mapper_number;

    uint8_t *rom;
    size_t   romlen;
    uint8_t *roms[16]{};
    uint8_t *prgrom_pages[16]{};
    uint8_t *chrrom_pages[16]{};

    size_t prgrom_page_len[16]{};
    size_t chrrom_page_len[16]{};

    int8_t prgrom_state[4]{};
    int8_t chrrom_state[16]{};

    vector<uint8_t> srams;

  public:
    Rom();
    ~Rom();

    void set_rom();
    void create_pages();
    void clear_roms();
    void set_prgrom_page_8k(int page, int rompage);
    void set_prgrom_page(int no, int num);

  private:
};

#endif    // _H_ROM_
