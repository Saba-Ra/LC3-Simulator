#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <QString>
#include "lc3memory.h"
#include "Files.h"


extern File globalFile;
class Assembler
{
public:
    Assembler();
};

#endif // ASSEMBLER_H

int startAssembly( QString inputFilename);

