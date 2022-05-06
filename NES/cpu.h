#include "mem.h"

using namespace std;

struct Opcode
{
    size_t opcode;
    size_t opint;
    string hex;
    string op;
    size_t adm;
    size_t cycle;
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
    size_t  cpuclock;
    size_t  cycles;
    size_t  total;

    size_t steps;
    size_t totalcycle;
    Mem   *mem;

  private:
    Opcode opcodes[258];

  public:
    Cpu(Mem *_mem);
    ~Cpu();

    void init();
    void init_nestest();
    void run(bool cputest);
    void reset();

  private:
    void create_opcode(size_t opcode, size_t opint, string hex, string op, size_t adm, size_t cycle);
    void create_opcodes();

    uint16_t get_addr(int mode);
    void     exe_instruction(size_t opint, uint16_t addr);

    void    set_zero_and_ng(uint8_t rval);
    void    setp(uint8_t value);
    uint8_t getp(bool bFlag);
    void    doBranch(bool test, uint16_t reladr);

    void show_state(uint16_t pc, string op, uint16_t adrm);
    void show_test_state(uint16_t pc, string op, uint16_t adrm);
};
