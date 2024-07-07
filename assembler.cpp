#include "assembler.h"
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QVector>
#include <QMap>
#include <QDebug>
#include <bitset>
#include <QRegularExpression>
#include <QMessageBox>

File globalFile("MEMORY.bin");

Assembler::Assembler()
{
}


QString checkFlag(const QString &token, const QChar &flag) {
    return token.contains(flag) ? "1" : "0";
}


// Helper function to split a string into tokens
QVector<QString> split(const QString &str, QChar delimiter)
{
    return str.split(delimiter).toVector();
}

// Function to convert an integer to a binary string with a specified number of bits
QString toBinary(int value, int bits)
{
    if (value < 0)
    {
        value = (1 << bits) + value; // Calculate two's complement for negative values
        return QString::fromStdString(std::bitset<16>(value).to_string()).right(bits).rightJustified(bits, '1');
    }
    else
    {
        return QString::fromStdString(std::bitset<16>(value).to_string()).right(bits).rightJustified(bits, '0');
    }
}


// First pass to identify labels and their corresponding memory addresses
QMap<QString, uint16_t> analyzeLabels(const QVector<QString> &lines)
{
    QMap<QString, uint16_t> labels;
    uint16_t startAddress = 0x3000; // Starting address
    for (const QString &line : lines)
    {
        if (line.isEmpty() || line.startsWith(';'))
            continue; // Ignore empty lines and comments

        QRegularExpression re("\\s+");
        QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);
        if (tokens[0] == "END")
        {
            break; // Stop processing at the end directive
        }
        else if (tokens[0] == "ORG")
        {
            bool flag;
            uint16_t newAddress =  tokens[1].toInt(&flag, 16);
            if (!flag)
            {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setWindowTitle("Invalid address Error");
                msgBox.setText("Failed to convert address");
                msgBox.setStyleSheet("QMessageBox { background-color: rgb(255, 220, 175); }"); // Light orange background
                msgBox.exec();

                exit;
            }
            else
            {
                startAddress = newAddress; // Set starting address
                qDebug() << "Setting address to:" << startAddress;
            }
            continue; // Skip further processing for ORG directive
        }

        else if (tokens[0].endsWith(',')) //This is a label
        {
            labels[tokens[0].chopped(1)] = startAddress;
            if (tokens.size() > 1)   ///////////////////////////////////
            {
                startAddress++; // If there's an instruction on the same line
            }
        }
        else //A regular assembly addresss
        {
            startAddress++;
        }
    }
    return labels;
}

QString handleDataDirectives(const QString &opcode, const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels, int currentAddress) {
    if (opcode == "DEC") {
        bool flag;
        int16_t value = tokens[1].toInt(&flag);
        if (!flag) {
            qWarning() << "Invalid DEC value:" << tokens[1];
            return "";
        }
        return toBinary(value, 16);
    }

    if (opcode == "HEX") {
        bool flag;
        uint16_t value = tokens[1].toUInt(&flag, 16);
        if (!flag) {
            qWarning() << "Invalid HEX value:" << tokens[1];
            return "";
        }
        return toBinary(value, 16);
    }

    if (opcode == "WORD") {
        QString result;
        for (int i = 1; i < tokens.size(); ++i) {
            bool flag;
            uint16_t value = tokens[i].toInt(&flag, 16);
            if (!flag) {
                qWarning() << "Invalid .WORD value:" << tokens[i];
                continue;
            }
            result += toBinary(value, 16) + "\n";
        }
        return result.trimmed();
    }

    if (opcode == "BYTE") {
        QString result;
        for (int i = 1; i < tokens.size(); ++i) {
            bool flag;
            uint8_t value = tokens[i].toInt(&flag, 16);
            if (!flag || value > 0xFF) {
                qWarning() << "Invalid .BYTE value:" << tokens[i];
                continue;
            }
            result += toBinary(value, 8).rightJustified(16, '0') + "\n"; // Pad to 16 bits
        }
        return result.trimmed();
    }

    return "";
}

// Function to assemble a single instruction into binary
QString assembleInstruction(const QString &instruction, const QMap<QString, uint16_t> &labels, uint16_t currentAddress)
{
    //QVector<QString> tokens = split(instruction, ' ');
    QRegularExpression re("\\s+");
    QVector<QString> tokens = instruction.split(re, Qt::SkipEmptyParts);
    for (int i = 0; i < tokens.size(); ++i)
    {
        tokens[i] = tokens[i].remove(',').trimmed(); // Remove commas and trim tokens
    }
    QString opcode = tokens[0];

    if (opcode == "ADD" || opcode == "AND")
    {
        QString res="";
        if(opcode == "ADD")
            res+="0001";
        else
            res+="0101";

        QString DR = toBinary(tokens[1].mid(1).toInt(), 3); // Destination register
        QString SR1 = toBinary(tokens[2].mid(1).toInt(), 3); // Source register 1
        if (tokens[3].startsWith('R')) //Register-to-Register
        {
            QString SR2 = toBinary(tokens[3].mid(1).toInt(), 3); // Source register 2
            return res + DR + SR1 + "000" + SR2;
        }
        else
        {
            // Immediate AND/ADD operation
            int imm5 = tokens[3].mid(1).toInt(); // Remove the '#' and convert to integer
            return res + DR + SR1 + "1" + toBinary(imm5, 5);
        }
    }

    if (opcode.startsWith("BR"))
    {

        QString n = checkFlag(tokens[0], 'n'); // Check if 'n' condition flag is present
        QString z = checkFlag(tokens[0], 'z'); // Check if 'z' condition flag is present
        QString p = checkFlag(tokens[0], 'p'); // Check if 'p' condition flag is present

        int relativeAddr = labels.value(tokens[1]) - currentAddress - 1;
        QString offsetBinary = toBinary(relativeAddr, 9);
        return "0000" + n + z + p + offsetBinary;
    }

    if (opcode == "JMP" || opcode == "JSRR")
    {
        QString BaseR = tokens[1];           // Register to jump to
        int BaseRNum = BaseR.mid(1).toInt(); // Remove 'R' and convert to int

        // Construct binary instruction
        QString binaryInstruction="";
        if (opcode == "JMP")
            binaryInstruction += "1100";
        else
            binaryInstruction += "0100";

        return binaryInstruction + "000" + toBinary(BaseRNum, 3) + "000000";
    }

    if (opcode == "JSR")
    {
        QString label = tokens[1];                        // Subroutine label

        // Calculate the offset from current address
        int16_t relativeAddr = labels.value(label) - currentAddress - 1;

        // Convert offset to binary with 11 bits
        QString offsetBinary = toBinary(relativeAddr, 11);

        // Construct the binary instruction
        QString binaryInstruction = "0100"; // Opcode for JSR
        binaryInstruction += "1";           // JSR indirect bit
        binaryInstruction += offsetBinary;
        return binaryInstruction;
    }

    if (opcode == "LD" || opcode == "LDI")
    {
        QString DR = toBinary(tokens[1].mid(1).toInt(), 3);        // Destination register
        int relativeAddr = labels.value(tokens[2]) - currentAddress - 1;
        QString res= "";
        if(opcode == "LD")
            res+="0010";
        else
            res+="1010";
        return res + DR + toBinary(relativeAddr, 9);
    }

    if (opcode == "LDR")
    {
        QString DR = toBinary(tokens[1].mid(1).toInt(), 3);
        QString BaseR = toBinary(tokens[2].mid(1).toInt(), 3);
        int relativeAddr = tokens[3].mid(1).toInt();
        return "0110" + DR + BaseR + toBinary(relativeAddr, 6);
    }

    if (opcode == "LEA")
    {
        QString DR = toBinary(tokens[1].mid(1).toInt(), 3); // Destination register
        int relativeAddr = labels.value(tokens[2]) - currentAddress - 1;
        return "1110" + DR + toBinary(relativeAddr, 9);
    }

    if (opcode == "NOT")
    {
        QString DR = toBinary(tokens[1].mid(1).toInt(), 3);
        QString SR = toBinary(tokens[2].mid(1).toInt(), 3);
        return "1001" + DR + SR + "111111";
    }

    if (opcode == "RET")
    {
        return "1100000111000000";
    }

    if (opcode == "ST" || opcode == "STI")
    {
        QString SR = toBinary(tokens[1].mid(1).toInt(), 3);             // Source register
        int relativeAddr = labels.value(tokens[2]) - currentAddress - 1; // Calculate offset
        QString offset = toBinary(relativeAddr, 9);
        QString res="";
        if (opcode == "ST")
            res+="0011";
        else
            res+="1011";

        return res + SR + offset;
    }

    if (opcode == "STR")
    {
        QString SR = toBinary(tokens[1].mid(1).toInt(), 3);    // Source register
        QString BaseR = toBinary(tokens[2].mid(1).toInt(), 3); // Base register
        int relativeAddr = tokens[3].mid(1).toInt();                // Offset (Remove the '#' and convert to integer)
        return "0111" + SR + BaseR + toBinary(relativeAddr, 6);
    }

    if (opcode == "HALT")
    {
        return "1111000000100101";
    }

    // Handle data directives
    QString result = handleDataDirectives(opcode, tokens, labels, currentAddress);
    if (!result.isEmpty()) {
        return result;
    }

    return "";
}


QVector<QString> splitWithoutComments(const QString &line, const QChar &separator)
{
    QVector<QString> tokens;
    QString currentToken;
    bool commentFound = false;

    for (int i = 0; i < line.size(); ++i)
    {
        QChar ch = line.at(i);

        if (ch == ';')
        { // If comment symbol found, break the loop
            commentFound = true;
            break;
        }
        else if (ch == separator)
        { // If separator found, add current token to tokens
            if (!currentToken.isEmpty())
            {
                tokens.append(currentToken.trimmed());
                currentToken.clear();
            }
        }
        else
        { // Otherwise, add character to current token
            currentToken += ch;
        }
    }

    // Add the last token if not empty
    if (!currentToken.isEmpty())
    {
        tokens.append(currentToken.trimmed());
    }

    // If a comment was found, ignore the rest of the line

    return tokens;
}

bool validateInstruction(const QVector<QString> &tokens, const QMap<QString, uint16_t> &labels)
{
    if (tokens.isEmpty())
        return false;

    QString opcode = tokens[0].trimmed(); // Trim any leading or trailing whitespace

    auto isRegister = [](const QString &token)
    {
        int regNum = token.at(1).digitValue(); // Get the digit after 'R'

        bool hey = token.startsWith('R') && regNum >= 0 && regNum <= 7;
        return hey;
    };

    if (opcode == "ADD" || opcode == "AND")
    {
        if (tokens.size() != 4)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
        if (tokens[3].startsWith('R'))
        {
            if (!isRegister(tokens[3]))
                return false;
        }
        else
        {
            bool ok;
            int imm = tokens[3].mid(1).toInt(&ok);
            if (!ok || imm < -16 || imm > 15)
                return false;
        }
    }
    else if (opcode.startsWith("BR"))
    {
        if (tokens.size() != 2)
            return false;
        if (!labels.contains(tokens[1]))
            return false;
    }
    else if (opcode == "JMP" || opcode == "JSRR")
    {
        if (tokens.size() != 2)
            return false;
        if (!isRegister(tokens[1]))
            return false;
    }
    else if (opcode == "JSR")
    {
        if (tokens.size() != 2)
            return false;
        if (!labels.contains(tokens[1]))
            return false;
    }
    else if (opcode == "LD" || opcode == "LDI" || opcode == "LEA" || opcode == "ST" || opcode == "STI")
    {
        if (tokens.size() != 3)
            return false;
        if (!isRegister(tokens[1]))
            return false;
        if (!labels.contains(tokens[2]))
            return false;
    }
    else if (opcode == "LDR" || opcode == "STR")
    {
        if (tokens.size() != 4)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
        bool ok;
        int offset = tokens[3].toInt(&ok);
        if (!ok || offset < -32 || offset > 31)
            return false;
    }
    else if (opcode == "NOT")
    {
        if (tokens.size() != 3)
            return false;
        if (!isRegister(tokens[1]) || !isRegister(tokens[2]))
            return false;
    }
    else if (opcode == "RET" || opcode == "HALT" || opcode == "END")
    {
        if (tokens.size() != 1)
            return false;
    }
    else if (opcode == "WORD" || opcode == "BYTE")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toUInt(&ok);
        if (!ok)
            return false;
    }
    else if (opcode == "DEC")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toInt(&ok);
        if (!ok)
            return false;
    }

    else if (opcode == "HEX")
    {
        if (tokens.size() != 2)
            return false;
        bool ok;
        tokens[1].toUInt(&ok, 16); // Validate hex value
        if (!ok)
            return false;
    }
    else
    {
        qWarning() << "Invalid opcode:" << opcode;
        return false;
    }

    return true;
}

// Second pass to convert assembly instructions to machine code
void analyzeInstructions(const QVector<QString> &lines, const QMap<QString, uint16_t> &labels, LC3Memory &memory)
{
    uint16_t address = 0x3000; // Starting address
    for (const QString &line : lines)
    {

        if (line.isEmpty() || line.startsWith(';'))
            continue; // Ignore empty lines and comments

        QRegularExpression re("\\s+"); //for removing extra spaces
        QVector<QString> tokens = line.split(re, Qt::SkipEmptyParts);

        if (tokens[0] == "END")
        {
            break; // End of the program
        }

        else if (tokens[0] == "ORG")
        {
            if(tokens.size() > 1){
                QString addrString = tokens[1];
                bool flag;
                uint16_t startingAddress = static_cast<uint16_t>(addrString.toInt(&flag, 16));
                if (!flag)
                {
                    QMessageBox msgBox;
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.setWindowTitle("Invalid address Error");
                    msgBox.setText("Failed to convert address");
                    msgBox.setStyleSheet("QMessageBox { background-color: rgb(255, 220, 175); }"); // Light orange background
                    msgBox.exec();

                    exit;
                }
                else
                {
                    address = startingAddress; // Set starting address
                    qDebug() << "Setting address to:" << address;
                }
            }

        }


        else if (tokens[0].endsWith(',')) //THis is a label
        {
            // Next token may be an instruction
            if (tokens.size() > 1)
            {
                // Extract the instruction part after the label and comma
                QString instruction = line.mid(line.indexOf(',') + 1).trimmed();

                // Split the instruction into tokens excluding comments
                QVector<QString> tokens2 = splitWithoutComments(instruction, ' ');

                if (validateInstruction(tokens2, labels))
                {
                    QString binaryInstruction = assembleInstruction(instruction, labels, address);
                    bool ok;
                    uint16_t machineCode = static_cast<uint16_t>(binaryInstruction.toUInt(&ok, 2));
                    if (!ok)
                    {
                        qWarning() << "Failed to convert binary instruction to machine code";
                        continue; // Skip writing invalid instruction to memory
                    }
                    memory.write(address, machineCode);
                    address++;
                }
                else
                {
                    qWarning() << "Skipping invalid instruction:" << line;
                }
            }
        }

        else
        {
            QVector<QString> tokens2 = splitWithoutComments(line, ' ');
            if (validateInstruction(tokens2, labels))
            { // validateInstruction(tokens, labels) fix
                // Regular instruction without label
                QString binaryInstruction = assembleInstruction(line, labels, address);

                bool flag;
                uint16_t machineCode = static_cast<uint16_t>(binaryInstruction.toUInt(&flag, 2));
                if (!flag)
                {
                    qWarning() << "Failed to convert binary instruction to machine code";
                    continue; // Skip writing invalid instruction to memory
                }

                memory.write(address, machineCode);
                address++;
            }
            else
            {
                qWarning() << "Skipping invalid instruction:" << line;
            }
        }
    }
}

// Main function to assemble
int startAssembly(QString inputFilename)
{

    File asmFile(inputFilename, 0);
    // Assemble the code
    QVector<QString> assemblyCode = asmFile.readFromassemblyFile();
    if (assemblyCode.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Assembly Error");
        msgBox.setText("Failed to assemble. Exiting...");
        msgBox.setStyleSheet("QMessageBox { background-color: rgb(255, 220, 175); }"); // Light orange background
        msgBox.exec();

        return 1; // Return error code
    }

    QMap<QString, uint16_t> labels = analyzeLabels(assemblyCode);

    LC3Memory temporary(0xFFFF); // Create memory with size 64KB

    analyzeInstructions(assemblyCode, labels, temporary);

    // Write assembled code to output file
    globalFile.writeToBinaryFile(temporary, 0x3000, 0x3000 + assemblyCode.size() - 1);

    QMessageBox msgBox;
    msgBox.setText("Assembly completed. Output written to MEMORY.bin :)");
    msgBox.setWindowTitle("Information");

    // Set the stylesheet for a light orange background and appropriate text color
    msgBox.setStyleSheet("QMessageBox { background-color: #FFD580; }"
                         "QLabel { color: black; }"
                         "QPushButton { background-color: #FFF2CC; color: black; }");

    msgBox.exec();
}
