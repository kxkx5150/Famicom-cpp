#include "apu.h"
#include <cassert>
#include <climits>
#include <cstdio>
#include <cstring>

constexpr uint8_t LEN_COUNTER_TABLE[]   = {10, 254, 20, 2,  40, 4,  80, 6,  160, 8,  60, 10, 14, 12, 26, 14,
                                           12, 16,  24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
constexpr bool    DUTY_CYCLE_TABLE[][8] = {
       {0, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 1, 0, 0, 0}, {1, 0, 0, 1, 1, 1, 1, 1}};
constexpr uint8_t  TRIANGLE_TABLE[]     = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5,  4,  3,  2,  1,  0,
                                           0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
constexpr uint16_t NOISE_PERIOD_TABLE[] = {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
constexpr uint32_t DMC_PERIOD_TABLE[]   = {428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54};
template <typename T> constexpr inline bool nth_bit(T x, uint8_t n)
{
    return (x >> n) & 1;
}

APU::APU()
{
    this->chan.pulse2.isPulse2 = true;
    this->power_cycle();
    this->filters[0] = new HiPassFilter(90, 96000);
    this->filters[1] = new HiPassFilter(440, 96000);
    this->filters[2] = new LoPassFilter(14000, 96000);
}
APU::~APU()
{
    for (FirstOrderFilter *filter : this->filters)
        delete filter;
}
APU::Mixer::Mixer()
{
    for (uint32_t i = 0; i < 31; i++)
        this->pulse_table[i] = 95.52 / (8128.0 / float(i) + 100);
    for (uint32_t i = 0; i < 203; i++)
        this->tnd_table[i] = 163.67 / (24329.0 / float(i) + 100);
}
float APU::Mixer::sample(uint8_t pulse1, uint8_t pulse2, uint8_t tri, uint8_t noise, uint8_t dmc) const
{
    return pulse_table[pulse1 + pulse2] + tnd_table[3 * tri + 2 * noise + dmc];
}
void APU::power_cycle()
{
    memset((char *)&this->chan, 0, sizeof this->chan);
    this->chan.noise.sr = 1;
    this->reset();
}
void APU::reset()
{
    this->cycles                    = 0;
    this->seq_step                  = 0;
    this->frame_counter.inhibit_irq = true;
}
uint8_t APU::read(uint16_t addr)
{
    // uint8_t retval = this->peek(addr);
    // if (addr == 0x4015) {
    //     this->frame_counter.inhibit_irq = true;
    // }
    // return retval;
    return 0;
}
uint8_t APU::peek(uint16_t addr)
{
    if (addr == 0x4015) {
        APU::StateReg state{0};
        state.dmc_irq   = !this->chan.dmc.inhibit_irq;
        state.frame_irq = !this->frame_counter.inhibit_irq;
        state.pulse1    = this->chan.pulse1.len_count.val > 0;
        state.pulse2    = this->chan.pulse2.len_count.val > 0;
        state.tri       = this->chan.tri.len_count.val > 0;
        state.noise     = this->chan.noise.len_count.val > 0;
        state.dmc       = this->chan.dmc.read_remaining > 0;
        return state.val;
    }

    printf("[APU] Peek from Write-Only register: 0x%04X\n", addr);
    return 0x00;
}
void APU::write(uint16_t addr, uint8_t val)
{

    switch (addr) {
        case 0x4000 + 0: {
            this->chan.pulse1.duty_cycle       = (val & 0xC0) >> 6;
            this->chan.pulse1.len_count.on     = !(val & 0x20);
            this->chan.pulse1.envelope.loop    = !!(val & 0x20);
            this->chan.pulse1.envelope.enabled = !(val & 0x10);
            this->chan.pulse1.envelope.period  = (val & 0x0F);
            this->chan.pulse1.envelope.reset   = true;
        } break;
        case 0x4004 + 0: {
            this->chan.pulse2.duty_cycle       = (val & 0xC0) >> 6;
            this->chan.pulse2.len_count.on     = !(val & 0x20);
            this->chan.pulse2.envelope.loop    = !!(val & 0x20);
            this->chan.pulse2.envelope.enabled = !(val & 0x10);
            this->chan.pulse2.envelope.period  = (val & 0x0F);
            this->chan.pulse2.envelope.reset   = true;
        } break;

        case 0x4000 + 1: {
            this->chan.pulse1.sweep._val  = val;
            this->chan.pulse1.sweep_reset = true;
        } break;
        case 0x4004 + 1: {
            this->chan.pulse2.sweep._val  = val;
            this->chan.pulse2.sweep_reset = true;
        } break;

        case 0x4000 + 2: {
            const uint16_t prev            = this->chan.pulse1.timer_period & 0xFF00;
            this->chan.pulse1.timer_period = prev | val;
        } break;
        case 0x4004 + 2: {
            const uint16_t prev            = this->chan.pulse2.timer_period & 0xFF00;
            this->chan.pulse2.timer_period = prev | val;
        } break;

        case 0x4000 + 3: {
            const uint16_t prev              = this->chan.pulse1.timer_period & 0x00FF;
            this->chan.pulse1.timer_period   = prev | ((val & 0x07) << 8);
            const uint16_t len_count         = LEN_COUNTER_TABLE[(val & 0xF8) >> 3];
            this->chan.pulse1.len_count.val  = len_count;
            this->chan.pulse1.envelope.reset = true;
            this->chan.pulse1.duty_val       = 0;
        } break;
        case 0x4004 + 3: {
            const uint16_t prev              = this->chan.pulse2.timer_period & 0x00FF;
            this->chan.pulse2.timer_period   = prev | ((val & 0x07) << 8);
            const uint16_t len_count         = LEN_COUNTER_TABLE[(val & 0xF8) >> 3];
            this->chan.pulse2.len_count.val  = len_count;
            this->chan.pulse2.envelope.reset = true;
            this->chan.pulse2.duty_val       = 0;
        } break;

        // Triangle
        case 0x4008: {
            this->chan.tri.len_count.on     = !(val & 0x80);
            this->chan.tri.lin_count_on     = !(val & 0x80);
            this->chan.tri.lin_count_period = (val & 0x7F);
        } break;
        case 0x400A: {
            const uint16_t prev         = this->chan.tri.timer_period & 0xFF00;
            this->chan.tri.timer_period = prev | val;
        } break;
        case 0x400B: {
            const uint16_t prev            = this->chan.tri.timer_period & 0x00FF;
            this->chan.tri.timer_period    = prev | ((val & 0x07) << 8);
            const uint16_t len_count       = LEN_COUNTER_TABLE[(val & 0xF8) >> 3];
            this->chan.tri.len_count.val   = len_count;
            this->chan.tri.timer_val       = this->chan.tri.timer_period;
            this->chan.tri.lin_count_reset = true;
        } break;

        // Noise
        case 0x400C: {
            this->chan.noise.len_count.on     = !(val & 0x20);
            this->chan.noise.envelope.loop    = !!(val & 0x20);
            this->chan.noise.envelope.enabled = !(val & 0x10);
            this->chan.noise.envelope.period  = (val & 0x0F);
            this->chan.noise.envelope.reset   = true;
        } break;
        case 0x400E: {
            this->chan.noise.mode         = !!(val & 0x80);
            this->chan.noise.timer_period = NOISE_PERIOD_TABLE[val & 0x0F];
        } break;
        case 0x400F: {
            const uint16_t len_count        = LEN_COUNTER_TABLE[(val & 0xF8) >> 3];
            this->chan.noise.len_count.val  = len_count;
            this->chan.noise.envelope.reset = true;
        } break;

        // DMC
        case 0x4010: {
            this->chan.dmc.inhibit_irq  = !(val & 0x80);
            this->chan.dmc.loop         = (val & 0x40);
            this->chan.dmc.timer_period = DMC_PERIOD_TABLE[val & 0x0F];
        } break;
        case 0x4011: {
            this->chan.dmc.output_val = (val & 0x7F);
        } break;
        case 0x4012: {
            this->chan.dmc.sample_addr = 0xC000 + (val << 6);
        } break;
        case 0x4013: {
            this->chan.dmc.sample_len = (val << 4) + 1;
        } break;

        // Control
        case 0x4015: {
            APU::StateReg state{val};
            this->chan.pulse1.enabled = state.pulse1;
            this->chan.pulse2.enabled = state.pulse2;
            this->chan.tri.enabled    = state.tri;
            this->chan.noise.enabled  = state.noise;
            this->chan.dmc.enabled    = state.dmc;
            if (!state.pulse1)
                this->chan.pulse1.len_count.val = 0;
            if (!state.pulse2)
                this->chan.pulse2.len_count.val = 0;
            if (!state.tri)
                this->chan.tri.len_count.val = 0;
            if (!state.noise)
                this->chan.noise.len_count.val = 0;

            if (!state.dmc)
                this->chan.dmc.read_remaining = 0;

            else if (!this->chan.dmc.read_remaining) {
                this->chan.dmc.read_addr      = this->chan.dmc.sample_addr;
                this->chan.dmc.read_remaining = this->chan.dmc.sample_len;

                if (!this->chan.dmc.output_sr) {
                    this->chan.dmc.dmc_transfer();
                } else {
                }
            }
        } break;

        // Frame Counter
        case 0x4017: {
            this->frame_counter.val = val;
            if (this->frame_counter.five_frame_seq) {
                this->clock_timers();
                this->clock_length_counters();
                this->clock_sweeps();
            }
        } break;
        default:
            break;
    }
}
void APU::Envelope::clock()
{
    if (this->reset) {
        this->val = 15;

        this->step  = this->period;
        this->reset = false;
        return;
    }

    if (this->step) {
        this->step--;
    } else {
        this->step = this->period;

        if (this->val)
            this->val--;
        else if (this->loop)
            this->val = 15;
    }
}
void APU::LenCount::clock()
{
    if (this->on && this->val) {
        this->val--;
    }
}
void APU::Channels::Pulse::timer_clock()
{
    if (this->timer_val) {
        this->timer_val--;
    } else {
        this->timer_val = this->timer_period;
        this->duty_val  = (this->duty_val + 1) % 8;
    }
}

void APU::Channels::Pulse::sweep_clock()
{
    if (this->sweep_reset) {
        this->sweep_val   = this->sweep.period + 1;
        this->sweep_reset = false;
        return;
    }

    if (this->sweep_val) {
        this->sweep_val--;
    } else {
        this->sweep_val = this->sweep.period + 1;
        if (this->sweep.enabled) {
            const uint16_t change = this->timer_period >> this->sweep.shift;
            if (!this->sweep.negate)
                this->timer_period += change;    // Normal
            else if (!this->isPulse2)
                this->timer_period -= change + 1;    // Neg 1
            else if (this->isPulse2)
                this->timer_period -= change + 0;    // Neg 2
        }
    }
}

uint8_t APU::Channels::Pulse::output() const
{
    const bool active = DUTY_CYCLE_TABLE[this->duty_cycle][this->duty_val];
    if (!this->enabled || !active || !this->len_count.val || this->timer_val < 8 || this->timer_period > 0x7FF)
        return 0;

    return this->envelope.enabled ? this->envelope.val : this->envelope.period;
}
void APU::Channels::Triangle::timer_clock()
{
    if (this->timer_val) {
        this->timer_val--;
    } else {
        this->timer_val = this->timer_period;
        this->duty_val  = (this->duty_val + 1) % 32;
    }
}

void APU::Channels::Triangle::lin_count_clock()
{
    if (this->lin_count_reset)
        this->lin_count_val = this->lin_count_period;
    else if (this->lin_count_val)
        this->lin_count_val--;

    if (this->lin_count_on)
        this->lin_count_reset = false;
}

uint8_t APU::Channels::Triangle::output() const
{
    if (!this->enabled || !this->len_count.val || !this->lin_count_val)
        return 0;

    return TRIANGLE_TABLE[this->duty_val];
}
void APU::Channels::Noise::timer_clock()
{
    if (this->timer_val) {
        this->timer_val--;
    } else {
        this->timer_val = this->timer_period;
        bool fb         = nth_bit(this->sr, 0) ^ nth_bit(this->sr, this->mode ? 6 : 1);
        this->sr >>= 1;
        this->sr |= fb << 14;
    }
}

uint8_t APU::Channels::Noise::output() const
{
    if (!this->enabled || !this->len_count.val || this->sr & 1)
        return 0;

    return this->envelope.enabled ? this->envelope.val : this->envelope.period;
}
void APU::Channels::DMC::dmc_transfer()
{
    // if (this->read_buffer_empty && this->read_remaining) {
    //     // 1) The CPU is stalled for up to 4 CPU cycles to allow the longest
    //     //    possible write (the return address and write after an IRQ) to
    //     //    finish. If OAM DMA is in progress, it is paused for two cycles.
    //     this->dmc_stall = true;
    //     // 2) The sample buffer is filled with the next sample byte read from
    //     //    the current address
    //     this->read_buffer       = mem[this->read_addr];
    //     this->read_buffer_empty = false;
    //     // 3) The address is incremented.
    //     //    If it exceeds $FFFF, it wraps to $8000
    //     if (++this->read_addr == 0x0000)
    //         this->read_addr = 0x8000;
    //     // 4) The bytes remaining counter is decremented;
    //     //    If it becomes zero and the loop flag is set, the sample is
    //     //    restarted; otherwise, if the bytes remaining counter becomes zero
    //     //    and the IRQ enabled flag is set, the interrupt flag is set.
    //     if (--this->read_remaining == 0) {
    //         if (this->loop) {
    //             this->read_addr      = this->sample_addr;
    //             this->read_remaining = this->sample_len;
    //         } else if (!this->inhibit_irq) {
    //             // this->inhibit_irq = true;
    //             interrupt.request(Interrupts::IRQ);
    //         }
    //     }
    // }
}

void APU::Channels::DMC::timer_clock()
{
    // if (this->timer_val) {
    //     this->timer_val--;
    //     return;
    // }
    // this->timer_val = this->timer_period;

    // // When the timer outputs a clock, the following actions occur in order:

    // // 1) If the silence flag is clear, the output level changes based on bit 0 of
    // //     the shift register; If the bit is 1, add 2; otherwise, subtract 2.
    // if (!this->output_silence) {
    //     // If adding or subtracting 2 would cause the output level to leave the
    //     //  0-127 range, leave the output level unchanged.
    //     if (this->output_sr & 1)
    //         this->output_val += (this->output_val <= 125) * 2;
    //     else
    //         this->output_val -= (this->output_val >= 2) * 2;
    //     // 2) The right shift register is clocked
    //     this->output_sr >>= 1;
    // }

    // // 3) The bits-remaining counter is updated.
    // //    When this counter reaches zero, we say that the output cycle ends.
    // if (this->output_bits_remaining) {
    //     this->output_bits_remaining--;
    // }
    // // When an output cycle ends, a new cycle is started as follows:
    // //  1) The bits-remaining counter is loaded with 8.
    // //  2) The sample buffer is emptied into the shift register, and is refilled
    // //      if needed.
    // if (!this->output_bits_remaining) {
    //     this->output_bits_remaining = 8;
    //     if (this->read_buffer_empty) {
    //         this->output_silence = true;
    //     } else {
    //         this->output_silence    = false;
    //         this->output_sr         = this->read_buffer;
    //         this->read_buffer_empty = true;
    //         // Perform memory read
    //         this->dmc_transfer(mem, interrupt);
    //     }
    // }
}

uint8_t APU::Channels::DMC::output() const
{
    if (!this->enabled)
        return 0;

    return this->output_val;
}
void APU::clock_timers()
{
    this->chan.tri.timer_clock();
    if (this->cycles % 2) {
        this->chan.pulse1.timer_clock();
        this->chan.pulse2.timer_clock();
        this->chan.noise.timer_clock();
        this->chan.dmc.timer_clock();
    }
}
void APU::clock_length_counters()
{
    this->chan.pulse1.len_count.clock();
    this->chan.pulse2.len_count.clock();
    this->chan.noise.len_count.clock();
    this->chan.tri.len_count.clock();
}
void APU::clock_sweeps()
{
    this->chan.pulse1.sweep_clock();
    this->chan.pulse2.sweep_clock();
}
void APU::clock_envelopes()
{
    this->chan.pulse1.envelope.clock();
    this->chan.pulse2.envelope.clock();
    this->chan.noise.envelope.clock();
    this->chan.tri.lin_count_clock();
}
void APU::cycle()
{
    this->cycles++;
    this->clock_timers();

    if (this->cycles % (this->clock_rate / 240) == 0) {
        if (this->frame_counter.five_frame_seq) {
            switch (this->seq_step % 5) {
                case 0: {
                    this->clock_envelopes();
                    this->clock_length_counters();
                    this->clock_sweeps();
                } break;
                case 1: {
                    this->clock_envelopes();
                } break;
                case 2: {
                    this->clock_envelopes();
                    this->clock_length_counters();
                    this->clock_sweeps();
                } break;
                case 3: {
                    this->clock_envelopes();
                } break;
                case 4: { /* nothing */
                } break;
            }
        } else {
            switch (this->seq_step % 4) {
                case 0: {
                    this->clock_envelopes();
                } break;
                case 1: {
                    this->clock_envelopes();
                    this->clock_length_counters();
                    this->clock_sweeps();
                } break;
                case 2: {
                    this->clock_envelopes();
                } break;
                case 3: {
                    this->clock_envelopes();
                    this->clock_length_counters();
                    this->clock_sweeps();
                    // if (this->frame_counter.inhibit_irq == false) {
                    //     this->interrupt.request(Interrupts::IRQ);
                    // }
                } break;
            }
        }
        this->seq_step++;
    }

    if (this->cycles % (this->clock_rate / 96000) == 0) {
        float sample = this->mixer.sample(this->chan.pulse1.output(), this->chan.pulse2.output(),
                                          this->chan.tri.output(), this->chan.noise.output(), this->chan.dmc.output());
        for (FirstOrderFilter *filter : this->filters)
            sample = filter->process(sample);

        audiobuff.data[audiobuff.i] = sample;
        if (audiobuff.i < 4095)
            audiobuff.i++;
    }
}

void APU::getAudiobuff(float **samples, uint32_t *len)
{
    if (samples == nullptr || len == nullptr)
        return;
    *samples          = this->audiobuff.data;
    *len              = this->audiobuff.i;
    this->audiobuff.i = 0;
}

void APU::set_speed(float speed)
{
    this->clock_rate = 1789773 * speed;
}
bool APU::stall_cpu()
{
    bool stall               = this->chan.dmc.dmc_stall;
    this->chan.dmc.dmc_stall = false;
    return stall;
}