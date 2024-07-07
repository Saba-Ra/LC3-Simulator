#include "Files.h"


LC3Memory memory(0xFFFF); // Create memory with size 0xFFFF (64KB)

File::File()
{

}
File::File(QString filename)
{
    binaryFile.setFileName(filename);
}
File::File(QString filename, int t)
{
    assemblyFile.setFileName(filename);
}

// Function to write machine code to an output file
void File::writeToBinaryFile(const LC3Memory& memory, uint16_t startAddress, uint16_t endAddress) {
    if (!binaryFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open file for writing:" << qPrintable("MEMORY.bin");
        return;
    }

    QDataStream output(&binaryFile);
    output.setVersion(QDataStream::Qt_5_0); // Set the data stream version if necessary

    for (uint16_t address = startAddress; address <= endAddress; address++) {
        uint16_t value = memory.read(address);
        output << value; // Write the machine code value to the binary file
    }

    binaryFile.close();
}

// Function to read instructions from a binary file and fill the memory
bool File::readFromBinaryFile(uint16_t startAddress) {
    if (!binaryFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file for reading:" << qPrintable("MEMORY.bin");
        return false;
    }

    QDataStream input(&binaryFile);
    input.setVersion(QDataStream::Qt_5_0); // Set the data stream version if necessary

    uint16_t address = startAddress;
    while (!input.atEnd()) {
        uint16_t value;
        input >> value; // Use the stream extraction operator to read a uint16_t value
        memory.write(address, value); // Write the value to memory at the current address
        address++;
    }

    binaryFile.close();
    return true;
}

// Function to read an assembly file and return its lines
QVector<QString> File::readFromassemblyFile()
{
    QVector<QString> lines;
    if (!assemblyFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Cannot open file for reading, " << "Error:" << assemblyFile.errorString();
        return lines;
    }
    QTextStream in(&assemblyFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        lines.append(line.trimmed());
    }
    assemblyFile.close(); // Always close the file after reading
    return lines;
}
