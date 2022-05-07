#include "nes.h"

static const int width = 256, height = 224;

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
void Nes::init()
{
    cpu->init();
}
void Nes::set_rom()
{
    string filename = "sm.nes";
    mapper->set_rom(filename);
}
void Nes::start(bool cputest)
{
    // cputest = true;
    size_t count = 0;
    if (cputest) {
        cpu->init_nestest();
        count = 8992;
    }else {
        init();
    }
    main_loop(count, cputest);
}
void Nes::main_loop(size_t count, bool cputest)
{
    SDL_Renderer *renderer;
    SDL_Texture  *MooseTexture;
    SDL_bool      done = SDL_FALSE;
    SDL_Window   *window =
        SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    renderer     = SDL_CreateRenderer(window, -1, 0);
    MooseTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    size_t i = 0;
    for (int Running = 1; Running;) {
        i++;
        if (i == count) {
            break;
        }

        cpu->run(cputest);
        ppu->run(cpu->cpuclock);
        cpu->clear_cpucycle();
        if (ppu->get_img_status()){
            auto imgdata = ppu->get_img_data();
            ppu->clear_img();
            UpdateTexture(MooseTexture, imgdata);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, MooseTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_QUIT)
                Running = 0;
        }
    }
}
void Nes::UpdateTexture(SDL_Texture *texture, uint32_t* imgdata)
{
    size_t     imgidx = 0;
    SDL_Color *color;
    Uint8     *src;
    Uint32    *dst;
    int        row, col;
    void      *pixels;
    int        pitch;
    SDL_LockTexture(texture, NULL, &pixels, &pitch);

    for (row = 0; row < height; ++row) {
        dst = (Uint32 *)((Uint8 *)pixels + row * pitch);
        for (col = 0; col < width; ++col) {
            *dst++ = imgdata[imgidx++];
        }
    }
    SDL_UnlockTexture(texture);
}
