#ifndef _H_PPU
#define _H_PPU

#include "irq.h"
#include "rom.h"
#include <fstream>


class Ppu {
  public:
    uint32_t imgdata[256 * 240]{};
    uint8_t  sprite_ram[0x100]{};

  private:
    Rom *rom = nullptr;
    Irq *irq = nullptr;

    size_t  ppux = 0;
    size_t  line = 0;
    uint8_t regs[8]{};

    bool   imgok  = false;
    size_t imgidx = 0;
    size_t rcount = 0;

    bool   sprite_zero      = false;
    bool   scroll_reg_flg   = false;
    size_t ppu_addr_buffer  = 0;
    size_t h_scroll_val     = 0;
    bool   ppu_addr_reg_flg = false;
    size_t ppu_addr         = 0;
    size_t ppu_read_buffer  = 0;

    int     screen_mirroring : HORIZONTAL;
    uint8_t vram[16][4096]{};
    uint8_t vrams[16][1024]{};

    uint8_t  bg_line_buffer[264]{};
    uint16_t sp_line_buffer[264]{};

    uint8_t palette[33]{};
    uint8_t spbit_pattern[256][256][8]{};

  public:
    Ppu(Rom *_rom, Irq *_irq);
    ~Ppu();
    void      init();
    void      start();
    void      reset();
    void      run(size_t cpuclock);
    void      clear_img();
    bool      get_img_status();
    uint32_t *get_img_data();
    void      set_chr_rom_page(size_t num);

  private:
    void clear_arryas();
    void crate_spbit_array();

    void set_mode_mirror(bool value);
    void init_mirrors(size_t value0, size_t value1, size_t value2, size_t value3);
    void set_chr_rom_data1k(size_t page, size_t romPage);
    void set_chrrom_pages1k(size_t rompage0, size_t rompage1, size_t rompage2, size_t rompage3, size_t rompage4,
                            size_t rompage5, size_t rompage6, size_t rompage7);

    void render_frame();
    void build_bg();
    void build_bg_line();
    void build_sp_line();

    uint8_t is_bigsize();
    bool    is_sprite_enable();
    bool    is_screen_enable();
    void    in_vblank();
    void    post_render();
    void    set_img_data(std::vector<uint8_t> rgb);

    // port
  public:
    void    write_scroll_reg(uint8_t value);
    void    write_ppu_ctrl0_reg(uint8_t value);
    void    write_ppu_ctrl1_reg(uint8_t value);
    uint8_t read_ppu_status_reg();
    void    write_ppu_addr_reg(uint8_t value);
    uint8_t read_ppu_data_reg();
    void    write_ppu_data_reg(uint8_t value);
    void    write_sprite_data(uint8_t value);
    void    write_sprite_addr_reg(uint8_t value);

  private:
    std::vector<uint8_t> PALLETE = {
        0x10, 0x01, 0x02, 0x03, 0x10, 0x05, 0x06, 0x07, 0x10, 0x09, 0x0a, 0x0b, 0x10, 0x0d, 0x0e, 0x0f,
    };

    std::vector<std::vector<uint8_t>> PALLETE_TABLE = {
        {101, 101, 101}, {0, 45, 105},    {19, 31, 127},   {60, 19, 124},   {96, 11, 98},    {115, 10, 55},
        {113, 15, 7},    {90, 26, 0},     {52, 40, 0},     {11, 52, 0},     {0, 60, 0},      {0, 61, 16},
        {0, 56, 64},     {0, 0, 0},       {0, 0, 0},       {0, 0, 0},       {174, 174, 174}, {15, 99, 179},
        {64, 81, 208},   {120, 65, 204},  {167, 54, 169},  {192, 52, 112},  {189, 60, 48},   {159, 74, 0},
        {109, 92, 0},    {54, 109, 0},    {7, 119, 4},     {0, 121, 61},    {0, 114, 125},   {0, 0, 0},
        {0, 0, 0},       {0, 0, 0},       {254, 254, 255}, {93, 179, 255},  {143, 161, 255}, {200, 144, 255},
        {247, 133, 250}, {255, 131, 192}, {255, 139, 127}, {239, 154, 73},  {189, 172, 44},  {133, 188, 47},
        {85, 199, 83},   {60, 201, 140},  {62, 194, 205},  {78, 78, 78},    {0, 0, 0},       {0, 0, 0},
        {254, 254, 255}, {188, 223, 255}, {209, 216, 255}, {232, 209, 255}, {251, 205, 253}, {255, 204, 229},
        {255, 207, 202}, {248, 213, 180}, {228, 220, 168}, {204, 227, 169}, {185, 232, 184}, {174, 232, 208},
        {175, 229, 234}, {182, 182, 182}, {0, 0, 0},       {0, 0, 0}};
};

#endif    // _H_PPU
