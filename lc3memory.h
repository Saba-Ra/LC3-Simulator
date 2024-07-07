#ifndef LC3MEMORY_H
#define LC3MEMORY_H

#include <cstdint>
#include <vector>


class LC3Memory
{
public:
    LC3Memory(uint16_t size);

    uint16_t read(uint16_t address) const;
    void write(uint16_t address, uint16_t value);

private:
    std::vector<uint16_t> memory;
};

#endif // LC3MEMORY_H
