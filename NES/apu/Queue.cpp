#include "Queue.h"

Queue::Queue()
{
    bufs       = NULL;
    free_sem   = NULL;
    write_buf  = 0;
    write_pos  = 0;
    read_buf   = 0;
    sound_open = false;
}
Queue::~Queue()
{
    if (sound_open) {
        SDL_PauseAudio(true);
        SDL_CloseAudio();
    }

    if (free_sem)
        SDL_DestroySemaphore(free_sem);

    delete[] bufs;
}
int Queue::sample_count() const
{
    int buf_free = SDL_SemValue(free_sem) * buf_size + (buf_size - write_pos);
    return buf_size * buf_count - buf_free;
}

const char *Queue::init(long sample_rate, int chan_count)
{
    bufs = new sample_t[(long)buf_size * buf_count];
    if (!bufs)
        return "error";

    free_sem = SDL_CreateSemaphore(buf_count - 1);
    if (!free_sem)
        return "error";

    SDL_AudioSpec as;
    as.freq     = sample_rate;
    as.format   = AUDIO_F32SYS;
    as.channels = chan_count;
    as.silence  = 0;
    as.samples  = buf_size;
    as.size     = 0;
    as.callback = fill_buffer_;
    as.userdata = this;
    if (SDL_OpenAudio(&as, NULL) < 0)
        return "error";
    SDL_PauseAudio(false);
    sound_open = true;

    return NULL;
}

inline Queue::sample_t *Queue::buf(int index)
{
    return bufs + (long)index * buf_size;
}
void Queue::write(const sample_t *in, int count)
{
    while (count) {
        int n = buf_size - write_pos;
        if (n > count)
            n = count;

        memcpy(buf(write_buf) + write_pos, in, n * sizeof(sample_t));
        in += n;
        write_pos += n;
        count -= n;

        if (write_pos >= buf_size) {
            write_pos = 0;
            write_buf = (write_buf + 1) % buf_count;
            SDL_SemWait(free_sem);
        }
    }
}
void Queue::fill_buffer(Uint8 *out, int count)
{
    if (SDL_SemValue(free_sem) < buf_count - 1) {
        memcpy(out, buf(read_buf), count);
        read_buf = (read_buf + 1) % buf_count;
        SDL_SemPost(free_sem);
    } else {
        memset(out, 0, count);
    }
}
void Queue::fill_buffer_(void *user_data, Uint8 *out, int count)
{
    ((Queue *)user_data)->fill_buffer(out, count);
}
