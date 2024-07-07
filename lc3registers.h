#ifndef LC3REGISTERS_H
#define LC3REGISTERS_H

#include <cstdint>

class LC3Registers
{
public:
    LC3Registers();

    uint16_t& set_get_PC();

    uint16_t& set_get_IR();

    uint16_t& set_get_CC();

    uint16_t getR(uint8_t index) const;
    void setR(uint8_t index, uint16_t value);

    uint16_t& set_get_MAR();

    uint16_t& set_get_MDR();


private:
    uint16_t PC;        // Program Counter
    uint16_t IR;        // Instruction Register
    uint16_t CC;        // Condition Code Register
    uint16_t R[8];      // General-purpose registers R0-R7
    uint16_t MAR;       // Memory Address Register
    uint16_t MDR;       // Memory Data Register
};

#endif // LC3REGISTERS_H
