#include "lc3memory.h"


LC3Memory::LC3Memory(uint16_t size)
{
    memory.resize(size);
}

uint16_t LC3Memory::read(uint16_t address) const
{
    if (address < memory.size()) {
        return memory[address];
    } else {
        // Handle error or throw exception
        return 0;
    }
}

void LC3Memory::write(uint16_t address, uint16_t value)
{
    if (address < memory.size()) {
        memory[address] = value;
    } else {
        // Handle error or throw exception
    }
}
