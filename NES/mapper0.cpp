#include "mapper0.h"

Mapper0::Mapper0(Rom *_rom)
{
    rom = _rom;
}
Mapper0::~Mapper0()
{
}
void Mapper0::set_rom()
{
    printf("Mapper0 set_rom\n");
    rom->set_rom();
    rom->set_prgrom_page(0, 0);
    rom->set_prgrom_page(1, rom->prg_rom_page_count - 1);
    // self.ppu.set_chr_rom_page(0, &mut self.rom);
    // self.ppu.start(&mut self.rom);
}
