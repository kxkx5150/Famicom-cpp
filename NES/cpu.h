#ifndef _H_CPU
#define _H_CPU

#include "irq.h"
#include "mem.h"



struct Opcode
{
    uint64_t opcode;
    uint64_t opint;
    std::string hex;
    std::string op;
    uint64_t adm;
    uint64_t cycle;
};

class Cpu {
  public:
    uint8_t  a;
    uint8_t  x;
    uint8_t  y;
    uint8_t  sp;
    uint16_t pc;

    bool negative;
    bool overflow;
    bool decimal;
    bool interrupt;
    bool zero;
    bool carry;

    uint8_t toirq;
    uint64_t  cpuclock;
    uint64_t  cycles;
    uint64_t  total;

    uint64_t steps;
    uint64_t totalcycle;
    Mem   *mem;
    Irq   *irq;

  private:
    Opcode opcodes[258];

  public:
    Cpu(Mem *_mem, Irq *_irq);
    ~Cpu();

    void init();
    void init_nestest();
    void exec_nmi();
    void exec_irq();

    void run(bool cputest);
    void reset();
    void clear_cpucycle();

  private:
    void create_opcode(uint64_t opcode, uint64_t opint, std::string hex, std::string op, uint64_t adm, uint64_t cycle);
    void create_opcodes();

    uint16_t get_addr(int mode);
    void     exe_instruction(uint64_t opint, uint16_t addr);

    void    set_zero_and_ng(uint8_t rval);
    void    setp(uint8_t value);
    uint8_t getp(bool bFlag);
    void    doBranch(bool test, uint16_t reladr);

    void show_state(uint16_t pc, std::string op, uint16_t adrm);
    void show_test_state(uint16_t pc, std::string op, uint16_t adrm);
};

#endif    // _H_CPU
