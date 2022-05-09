#ifndef _H_APU
#define _H_APU

#include "bitfield.h"
#include "filters.h"

class APU {
  public:
    APU();
    ~APU();
    uint8_t read(uint16_t addr);
    uint8_t peek(uint16_t addr);
    void    write(uint16_t addr, uint8_t val);

    void    power_cycle();
    void    reset();
    void    cycle();
    bool    stall_cpu();
    void    getAudiobuff(float **samples, uint32_t *len);
    void    set_speed(float speed);

  private:
    struct Envelope
    {
        bool    enabled;
        uint8_t period;
        bool    loop;
        bool    reset;
        uint8_t step;
        uint8_t val;
        void    clock();
    };

    struct LenCount
    {
        bool    on;
        uint8_t val;
        void    clock();
    };
    struct Channels
    {
        struct Pulse
        {
            bool     enabled;
            Envelope envelope;
            LenCount len_count;
            uint8_t  duty_cycle;
            union
            {
                uint8_t        _val;
                BitField<7>    enabled;
                BitField<4, 3> period;
                BitField<3>    negate;
                BitField<0, 3> shift;
            } sweep;
            uint16_t timer_period;
            bool     sweep_reset;
            uint8_t  sweep_val;
            uint8_t  duty_val;
            uint16_t timer_val;

            void sweep_clock();
            void timer_clock();

            uint8_t output() const;
            bool    isPulse2;
        } pulse1, pulse2;

        struct Triangle
        {
            bool     enabled;
            LenCount len_count;

            bool     lin_count_on;
            uint8_t  lin_count_period;
            uint16_t timer_period;

            bool     lin_count_reset;
            uint8_t  lin_count_val;
            uint8_t  duty_val;
            uint16_t timer_val;

            void    timer_clock();
            void    lin_count_clock();
            uint8_t output() const;
        } tri;

        struct Noise
        {
            bool     enabled;
            Envelope envelope;
            LenCount len_count;
            bool     mode;
            uint16_t timer_period;
            uint16_t sr;
            uint16_t timer_val;
            void     timer_clock();
            uint8_t  output() const;
        } noise;

        struct DMC
        {
            bool enabled;

            bool     inhibit_irq;
            bool     loop;
            uint32_t timer_period;
            uint8_t  output_val;
            uint16_t sample_addr;
            uint16_t sample_len;

            uint16_t timer_val;
            uint8_t  read_buffer;
            bool     read_buffer_empty;
            uint16_t read_addr;
            uint32_t read_remaining;
            uint8_t  output_sr;
            uint32_t output_bits_remaining;
            bool     output_silence;
            bool     dmc_stall;
            void     timer_clock();
            void     dmc_transfer();
            uint8_t  output() const;
        } dmc;
    } chan;

    union StateReg
    {
        uint8_t     val;
        BitField<7> dmc_irq;
        BitField<6> frame_irq;
        BitField<4> dmc;
        BitField<3> noise;
        BitField<2> tri;
        BitField<1> pulse2;
        BitField<0> pulse1;
    };

    union FrameCounter
    {
        uint8_t     val;
        BitField<7> five_frame_seq;
        BitField<6> inhibit_irq;
    } frame_counter;

    FirstOrderFilter *filters[3];
    uint32_t          cycles;
    uint32_t          seq_step;

    struct
    {
        uint32_t i          = 0;
        float    data[4096] = {0};
    } audiobuff;

    uint32_t clock_rate = 1789773;

  private:
    void clock_envelopes();
    void clock_sweeps();
    void clock_timers();
    void clock_length_counters();
    class Mixer {
      private:
        float pulse_table[31];
        float tnd_table[203];

      public:
        Mixer();
        float sample(uint8_t pulse1, uint8_t pulse2, uint8_t triangle, uint8_t noise, uint8_t dmc) const;
    } mixer;
};

#endif
