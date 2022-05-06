#include "NES/nes.h"
#include <math.h>

int main(int ArgCount, char **Args)
{
    auto nes = new Nes();
    nes->set_rom();
    nes->start();
    delete nes;
    return 0;
}