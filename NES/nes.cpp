#include "nes.h"

Nes::Nes()
{
    rom    = new Rom();
    mapper = new Mapper0(rom);
    mem    = new Mem(mapper);
    cpu    = new Cpu(mem);
}
Nes::~Nes()
{
    delete rom;
    delete mapper;
    delete mem;
    delete cpu;
}
void Nes::set_rom()
{
    mapper->set_rom();
}
void Nes::start(bool cputest)
{
    size_t count = 0;
    if (cputest) {
        cpu->init_nestest();
        // count = 5;
        count = 8991;
    } else {
    }
    main_loop(count, cputest);
}
void Nes::main_loop(size_t count, bool cputest)
{
    size_t i = 0;
    while (count == 0 || count != i) {
        i++;
        cpu->run(cputest);
    }
}
