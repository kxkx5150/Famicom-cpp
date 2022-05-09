#ifndef SOUND_QUEUE_H
#define SOUND_QUEUE_H

#include <SDL2/SDL.h>

class Queue {
  public:
    Queue();
    ~Queue();

    const char   *init(long sample_rate, int chan_count = 1);
    int           sample_count() const;
    typedef float sample_t;
    void          write(const sample_t *, int count);

  private:
    enum
    {
        buf_size = 2048
    };
    enum
    {
        buf_count = 3
    };
    sample_t *volatile bufs;
    SDL_sem *volatile free_sem;
    int volatile read_buf;
    int  write_buf;
    int  write_pos;
    bool sound_open;

    sample_t   *buf(int index);
    void        fill_buffer(Uint8 *, int);
    static void fill_buffer_(void *, Uint8 *, int);
};

#endif
