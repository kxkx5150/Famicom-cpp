#include "mapper0.h"
#include <cstdio>

Mapper0::Mapper0(Rom *_rom, Ppu *_ppu)
{
    rom = _rom;
    ppu = _ppu;
}
Mapper0::~Mapper0()
{
}
void Mapper0::init()
{
    printf("Mapper0 init\n");
    ppu->init();
}
void Mapper0::set_rom(string filename)
{
    printf("Mapper0 set_rom\n");
    rom->set_rom(filename);
    rom->set_prgrom_page(0, 0);
    rom->set_prgrom_page(1, rom->prg_rom_page_count - 1);
    ppu->set_chr_rom_page(0);
    ppu->start();
}
