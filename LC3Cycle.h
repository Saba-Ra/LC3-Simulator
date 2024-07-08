#ifndef LC3CYCLE_H
#define LC3CYCLE_H

#include "lc3registers.h"
#include "lc3memory.h"

extern uint16_t ir;
extern uint16_t nzp;
extern uint16_t dr;
extern uint16_t sr1;
extern uint16_t imm_flag;
extern uint16_t sr2;
extern uint16_t imm5;
extern int16_t offset9;
extern int16_t PCoffset6;
extern int16_t offset11;
extern uint16_t BaseR;
extern uint16_t flag;
extern uint16_t opcode;
extern uint16_t address;
extern uint16_t operand1;
extern uint16_t operand2;
extern uint16_t GateALU;
extern uint16_t value;
extern uint16_t SR;
class LC3Cycle
{
public:
    static void fetch( LC3Memory& memory);
    static void decode();
    static void evaluateAddress( LC3Memory& memory);
    static void fetchOperands(LC3Memory& memory);
    static void execute();
    static void  storeResults(LC3Memory &memory);
    static bool isHalt();
    static void ANDorADDdecode();


};

#endif // LC3CYCLE_H
