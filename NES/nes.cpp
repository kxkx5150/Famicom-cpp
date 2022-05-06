#include "nes.h"

Nes::Nes()
{
    rom    = new Rom();
    irq    = new Irq();
    ppu    = new Ppu(rom, irq);
    mapper = new Mapper0(rom, ppu);
    mem    = new Mem(mapper);
    cpu    = new Cpu(mem, irq);
}
Nes::~Nes()
{
    delete rom;
    delete irq;
    delete ppu;
    delete mapper;
    delete mem;
    delete cpu;
}
void Nes::set_rom()
{
    string filename = "sm.nes";
    mapper->set_rom(filename);
}
void Nes::start(bool cputest)
{
    size_t count = 0;
    if (cputest) {
        cpu->init_nestest();
        count = 8991;
    } else {
        cpu->init();
    }
    main_loop(count, cputest);
}
void Nes::main_loop(size_t count, bool cputest)
{
    size_t i = 0;
    while (count == 0 || count != i) {
        i++;
        cpu->run(cputest);
        ppu->run(cpu->cpuclock);
        cpu->clear_cpucycle();

    }
}
