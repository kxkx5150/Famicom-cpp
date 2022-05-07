#ifndef _H_IRQ
#define _H_IRQ

#include <string>

class Irq {
    bool nmi = false;
    bool irq = false;

  public:
    Irq()
    {
        nmi = false;
        irq = false;
    }
    ~Irq()
    {
    }

    void set_nmi(bool flg)
    {
        nmi = flg;
    }
    bool get_nmi()
    {
        return nmi;
    }
    void set_irq(bool flg)
    {
        irq = flg;
    }
    bool get_irq()
    {
        return irq;
    }

    std::string check_interrupt(bool interrupt)
    {
        if (nmi) {
            return "nmi";
        } else if (interrupt && irq) {
            return "irq";
        } else {
            return "";
        }
    }
    void clear_nmi()
    {
        nmi = false;
    }
    void clear_irq()
    {
        irq = false;
    }
    void clear()
    {
        nmi = false;
        irq = false;
    }
};

#endif
