#ifndef _H_IO
#define _H_IO

#include <fstream>

class Io {
    uint8_t ctrlstat1   = 0;
    uint8_t ctrlstat2   = 0;
    uint8_t latchstate1 = 0;
    uint8_t latchstate2 = 0;
    bool    ctrllatched = false;

  public:
    void init()
    {
        reset();
    }
    void reset()
    {
        ctrlstat1   = 0;
        ctrlstat2   = 0;
        latchstate1 = 0;
        latchstate2 = 0;
        ctrllatched = false;
    }

    void set_ctrllatched(bool state)
    {
        ctrllatched = state;
    }
    bool get_ctrllatched()
    {
        return ctrllatched;
    }

    void set_ctrlstat1(uint8_t flg)
    {
        ctrlstat1 = flg;
    }
    uint8_t get_ctrlstat1()
    {
        return ctrlstat1;
    }
    void set_ctrlstat2(uint8_t flg)
    {
        ctrlstat2 = flg;
    }
    uint8_t get_ctrlstat2()
    {
        return ctrlstat2;
    }

    void hdCtrlLatch()
    {
        latchstate1 = ctrlstat1;
        latchstate2 = ctrlstat2;
    }

    void set_latched_ctrl_state(uint8_t no)
    {
        if (no == 1) {
            uint8_t val = latchstate1;
            val >>= 1;
            val |= 0x80;
            latchstate1 = val;
        } else {
            uint8_t val = latchstate2;
            val >>= 1;
            val |= 0x80;
            latchstate2 = val;
        }
    }
    uint8_t get_latched_ctrl_state(uint8_t no)
    {
        if (no == 1) {
            return latchstate1;
        } else {
            return latchstate2;
        }
    }
};

#endif
