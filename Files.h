#ifndef FILES_H
#define FILES_H
#include <QFile>
#include <QString>
#include <QDebug>
#include "lc3memory.h"

extern LC3Memory memory;

class File
{
public:
    File();
    File(QString);
    File(QString, int);
    void writeToBinaryFile(const LC3Memory&, uint16_t, uint16_t);
    bool readFromBinaryFile(uint16_t);
    QVector<QString> readFromassemblyFile();

    QFile binaryFile;
    QFile assemblyFile;
};

#endif // FILES_H
