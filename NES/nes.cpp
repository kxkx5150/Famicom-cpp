#include "nes.h"
#include <cstdint>

const int     width = 256, height = 224;
const uint8_t PAD_A      = 0x01;
const uint8_t PAD_B      = 0x02;
const uint8_t PAD_SELECT = 0x04;
const uint8_t PAD_START  = 0x08;
const uint8_t PAD_U      = 0x10;
const uint8_t PAD_D      = 0x20;
const uint8_t PAD_L      = 0x40;
const uint8_t PAD_R      = 0x80;

Nes::Nes()
{
    io     = new Io();
    rom    = new Rom();
    irq    = new Irq();
    ppu    = new Ppu(rom, irq);
    mapper = new Mapper0(rom, ppu);
    dma    = new Dma();
    mem    = new Mem(mapper, dma, io);
    cpu    = new Cpu(mem, irq);
}
Nes::~Nes()
{
    delete io;
    delete rom;
    delete irq;
    delete ppu;
    delete mapper;
    delete dma;
    delete mem;
    delete cpu;
}
void Nes::init()
{
    mem->init();    
}
void Nes::set_rom()
{
    string filename = "j.nes";
    mapper->set_rom(filename);
}
void Nes::start(bool cputest)
{
    cpu->init();
    // cputest = true;
    size_t count = 0;
    if (cputest) {
        cpu->init_nestest();
        count = 8992;
    }
    main_loop(count, cputest);
}
void Nes::main_loop(size_t count, bool cputest)
{
    SDL_Renderer *renderer;
    SDL_Texture  *MooseTexture;
    SDL_bool      done = SDL_FALSE;
    SDL_Window   *window =
        SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * 2, height * 2, SDL_WINDOW_SHOWN);
    renderer     = SDL_CreateRenderer(window, -1, 0);
    MooseTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_RenderSetScale(renderer, 2.0, 2.0);
    SDL_Event event;

    size_t  i    = 0;
    bool    quit = false;
    uint8_t pad  = 0;

    while (!quit) {
        i++;
        if (i == count)
            break;

        if (io->get_ctrllatched()) {
            io->hdCtrlLatch();
        }
        cpu->run(cputest);
        ppu->run(cpu->cpuclock);
        cpu->clear_cpucycle();
        if (ppu->get_img_status()) {
            auto imgdata = ppu->get_img_data();
            ppu->clear_img();
            UpdateTexture(MooseTexture, imgdata);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, MooseTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    pad |= keycode_to_pad(event);
                    io->set_ctrlstat1(pad);
                    break;
                case SDL_KEYUP: {
                    pad &= ~keycode_to_pad(event);
                    io->set_ctrlstat1(pad);
                    break;
                }
            }
        }
    }
}
uint8_t Nes::keycode_to_pad(SDL_Event event)
{
    auto key = event.key.keysym.sym;
    switch (key) {
        case SDLK_x:
            return PAD_A;
        case SDLK_z:
            return PAD_B;
        case SDLK_a:
            return PAD_SELECT;
        case SDLK_s:
            return PAD_START;
        case SDLK_RIGHT:
            return PAD_R;
        case SDLK_LEFT:
            return PAD_L;
        case SDLK_UP:
            return PAD_U;
        case SDLK_DOWN:
            return PAD_D;
        default:
            return 0;
    }
}
void Nes::UpdateTexture(SDL_Texture *texture, uint32_t *imgdata)
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
