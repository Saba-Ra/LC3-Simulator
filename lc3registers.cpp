#include "lc3registers.h"

LC3Registers::LC3Registers()
{
    PC = 0;
    IR = 0;
    CC = 0;
    for (int i = 0; i < 8; ++i) {
        R[i] = 0;
    }
    MAR = 0;
    MDR = 0;
}


uint16_t& LC3Registers::set_get_PC() {
    return PC;
}

uint16_t& LC3Registers::set_get_IR() {
    return IR;
}

uint16_t& LC3Registers::set_get_CC() {
    return CC;
}


uint16_t LC3Registers::getR(uint8_t index) const
{
    if (index < 8) {
        return R[index];
    } else {
        // Handle error or throw exception
        return 0;
    }
}

void LC3Registers::setR(uint8_t index, uint16_t value)
{
    if (index < 8) {
        R[index] = value;
    } else {
        // Handle error or throw exception
    }
}

uint16_t& LC3Registers::set_get_MAR() {
    return MAR;
}

uint16_t& LC3Registers::set_get_MDR() {
    return MDR;
}
