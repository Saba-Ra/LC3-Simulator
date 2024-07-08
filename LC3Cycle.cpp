#include "LC3Cycle.h"
#include <cstdint>
#include "LC3.h"
uint16_t ir;
uint16_t nzp;
uint16_t DR;
uint16_t SR1;
uint16_t is_imm;
uint16_t SR2;
uint16_t imm5;
int16_t PCoffset9;
int16_t PCoffset6;
int16_t PCoffset11;
uint16_t BaseR;
uint16_t flag;
uint16_t opcode;
uint16_t address;
uint16_t operand1;
uint16_t operand2;
uint16_t GateALU;
uint16_t value;
uint16_t SR;



void LC3Cycle::fetch(LC3Memory& memory)
{
    uint16_t pc = registers.set_get_PC();
    registers.set_get_MAR()=pc;
    registers.set_get_MDR()=memory.read(pc);
    registers.set_get_PC()=pc + 1;
    registers.set_get_IR()=registers.set_get_MDR();
}
void LC3Cycle::decode()
{
    opcode = (registers.set_get_IR() >> 12) & 0xF; //TO extract 4 most significiant bits

    switch (opcode)
    {
    case 0x0: // // BR Label
    {
        ir = registers.set_get_IR();
        nzp = (ir >> 9) & 0x7; // Extract nzp bits
        PCoffset9 = ir & 0x1FF;// Extract PCoffset9 (9 least significant bit)
        if (PCoffset9 & 0x100) //check to see if this is a negative value
        {
            PCoffset9 |= 0xFE00; // Sign extend to the left => add 7 '1' digit to the left of the offset
        }
        break;
    }

    case 0x1: //ADD DR, SR1, SR2     ADD DR, SR1, imm5
    {
        ANDorADDdecode();
        break;
    }

    case 0x2: //LD DR, LABEL
    {
        DR = (registers.set_get_IR() >> 9) & 0x0007;
        PCoffset9 = registers.set_get_IR() & 0x01FF;
        if (PCoffset9 & 0x0100) // Sign extension
        {
            PCoffset9 |= 0xFE00; //1111 1110 0000 0000 + PCoffset9
        }
        break;
    }

    case 0x3: //ST SR, LABEL
    {
        SR = (registers.set_get_IR() >> 9) & 0x7;
        PCoffset9 = registers.set_get_IR() & 0x1FF;
        if (PCoffset9 & 0x0100) // Sign extension
        {
            PCoffset9 |= 0xFE00;
        }
        break;
    }

    case 0x4: //JSR, JSRR
    {
        flag = (registers.set_get_IR() >> 11) & 0x1;
        if (flag){
            //JSR LABEL
            PCoffset11 = registers.set_get_IR() & 0x7FF; // PCoffset11 & 0000 0111 1111 1111
            if (PCoffset11 & 0x0400) // Sign extension
            {
                PCoffset11 |= 0xF800; //PCoffset11 OR  1111 1000 0000 0000
            }
        }else{
            //JSRR BaseR
            BaseR = (registers.set_get_IR() >> 6) & 0x7;
        }
        break;
    }

    case 0x5: //AND DR, SR1, SR2    AND DR, SR1, imm5
    {
        ANDorADDdecode();
        break;
    }

    case 0x6: //LDR, DR, BaseR, offset6
    {
        DR = (registers.set_get_IR() >> 9) & 0x0007;
        BaseR = (registers.set_get_IR() >> 6) & 0x0007;
        PCoffset6 = registers.set_get_IR() & 0x003F; //PCoffset6 & 0000 0000 0011 1111
        if (PCoffset6 & 0x0020) // Sign extension
        {
            PCoffset6 |= 0xFFC0; //PCoffset6 OR 1111 1111 1100 0000
        }
        break;
    }

    case 0x7: //STR SR, BaseR, offset6
    {
        SR = (registers.set_get_IR() >> 9) & 0x7;
        BaseR = (registers.set_get_IR() >> 6) & 0x7;
        PCoffset6 = registers.set_get_IR() & 0x3F; //PCoffset6 & 0000 0000 0011 1111
        if (PCoffset6 & 0x0020) // Sign extension
        {
            PCoffset6 |= 0xFFC0; //PCoffset6 OR 1111 1111 1100 0000
        }
        break;
    }

    case 0x9: //NOT DR, SR
    {
        DR = (registers.set_get_IR() >> 9) & 0x7; // destination register
        SR = (registers.set_get_IR() >> 6) & 0x7; // source register
        break;
    }

    case 0xA: //LDI DR, LABEL
    {
        DR = (registers.set_get_IR() >> 9) & 0x0007;
        PCoffset9 = registers.set_get_IR() & 0x01FF; //PCoffset9 & 0000 0001 1111 1111
        if (PCoffset9 & 0x0100) // Sign extension
        {
            PCoffset9 |= 0xFE00; //PCoffset9 OR 1111 1110 0000 0000
        }
        break;
    }

    case 0xB: //STI SR, LABEL
    {
        SR = (registers.set_get_IR() >> 9) & 0x7;
        PCoffset9 = registers.set_get_IR() & 0x1FF; //PCoffset9 & 0000 0001 1111 1111
        if (PCoffset9 & 0x0100) // Sign extension
        {
            PCoffset9 |= 0xFE00; //PCoffset9 OR 1111 1110 0000 0000
        }
        break;
    }

    case 0xC:  //RET
    {
        if (((registers.set_get_IR() >> 6) & 0x7) == 7) { // Check if it's a RET instruction
            //RET

        } else {
            //JMP
            BaseR = (registers.set_get_IR() >> 6) & 0x7;
        }
        break;
    }

    case 0xD: //reserved

    case 0xE: //LEA DR, LABEL
    {
        DR = (registers.set_get_IR() >> 9) & 0x0007;
        PCoffset9 = ir & 0x1FF; //PCoffset9 & 0000 0001 1111 1111
        if (PCoffset9 & 0x100)
        {
            PCoffset9 |= 0xFE00; //PCoffset9 OR 1111 1110 0000 0000
        }
        break;
    }

    default:
        break;
    }
}
void LC3Cycle::evaluateAddress(LC3Memory &memory)
{

    switch (opcode) {
    case 0x0:
        //BR => PC = PC + SEXT(PCOFFSET9)
        address=registers.set_get_PC() + PCoffset9;
        break;
    case 0x2:
        // LD => DR = mem[PC + SEXT(PCoffset9)]
        address = registers.set_get_PC() + PCoffset9;
        // Set the address in MAR
        registers.set_get_MAR()=address;
        break;
    case 0x3:
        // ST => mem[PC + SEXT(PCoffset9)] = SR
        address = registers.set_get_PC() + PCoffset9;
        // Set the address in MAR
        registers.set_get_MAR()=address;
        break;
    case 0x4:
        //JSR => PC = PC + SEXT(PCOFFSET11)
        if (flag){
            address=registers.set_get_PC() + PCoffset11;
        }else{
            //JSRR => PC = BaseR
            address=registers.getR(BaseR);
        }
        break;
    case 0x6:
        // LDR => DR = mem[BaseR + SEXT(offset6)]
        address = registers.getR(BaseR) + PCoffset6;
        registers.set_get_MAR()=address;
        break;
    case 0x7:
        // STR => mem[BaseR + SEXT(offset6)] = SR
        address = registers.getR(BaseR) + PCoffset6;
        // Set the address in MAR
        registers.set_get_MAR()=address;
        break;
    case 0xA:
        // LDI => DR = mem[mem[PC + SEXT(PCoffset9)]]
        address = registers.set_get_PC() + PCoffset9;
        registers.set_get_MAR()=address;
        address = memory.read(registers.set_get_MAR());
        registers.set_get_MAR()=address;
        break;
    case 0xB:
        // STI => mem[mem[PC + SEXT(PCoffset9)]] = SR
        address = registers.set_get_PC() + PCoffset9;
        // Set the address in MAR
        registers.set_get_MAR()=address;
        break;
    case 0xC:
        //RET
    {
        if (((registers.set_get_IR() >> 6) & 0x7) == 7) {
            // PC = R7
            address=registers.getR(7);
        }
        //JMP => PC = BaseR
        else {
            address=registers.getR(BaseR);
        }
        break;
    }
    case 0xE:
        // LEA => DR = PC + SEXT(PCoffset9)
        address = registers.set_get_PC() + PCoffset9;
        break;
    default:
        break;
    }
}
void LC3Cycle::fetchOperands(LC3Memory &memory)
{
    switch (opcode)
    {
    case 0x1:
        // ADD
        operand1 = registers.getR(SR1);
        operand2 = registers.getR(SR2);
        break;
    case 0x2:
        // LD
        registers.set_get_MDR()=memory.read(registers.set_get_MAR());
        break;
    case 0x3:
        //ST
        value=registers.getR(SR);
        break;
    case 0x5:
        // AND
        operand1 = registers.getR(SR1);
        operand2 = registers.getR(SR2);
        break;
    case 0x6:
        // LDR
        registers.set_get_MDR()=memory.read(registers.set_get_MAR());
        break;
    case 0x7:
        //STR
        value=registers.getR(SR);
    case 0x9:
        //NOT
        operand1=registers.getR(SR);
    case 0xA:
        // LDI
        registers.set_get_MDR()=memory.read(registers.set_get_MAR());
        break;
    case 0xB:
        //STI
        value=registers.getR(SR);
        break;
    default:
        break;
    }
}
void LC3Cycle::execute()
{
    switch (opcode)
    {
    case 0x1:
        // ADD
        // Calculate GateALU based on the mode (Immediate or Register)
        GateALU = is_imm ? (operand1 + static_cast<int16_t>(imm5)) : (operand1 + operand2);
        break;
    case 0x5:
        // AND
        // Calculate GateALU based on the mode (Immediate or Register)
        GateALU = is_imm ? (registers.getR(SR1) & static_cast<int16_t>(imm5)) : (registers.getR(SR1) & registers.getR(SR2));
        break;
    case 0x9:
        //NOT
        GateALU=~operand1;
        break;
    default:
        break;
    }
}
void LC3Cycle::storeResults(LC3Memory &memory)
{
    switch (opcode)
    {
    case 0x0:
    { // BR
        uint16_t cc = registers.set_get_CC();
        // Check if any of the conditions are met
        bool condition = ((nzp & 0x4) && (cc & 0x4)) || // n
                ((nzp & 0x2) && (cc & 0x2)) || // z
                ((nzp & 0x1) && (cc & 0x1));   // p
        if (condition)
        {
            registers.set_get_PC()=address;
        }
        break;
    }
    case 0x1:
        // ADD
    {
        registers.setR(DR, GateALU);

        uint16_t result = registers.getR(DR);
        registers.set_get_CC() = (result == 0) ? 0x02 : ((result >> 15) ? 0x04 : 0x01);
        break;
    }
    case 0x2:
        // LD
    {
        // Read value from MDR
        uint16_t value = registers.set_get_MDR();
        // Store value in destination register
        registers.setR(DR, value);
        // Update condition codes
        if (value == 0)
        {
            registers.set_get_CC()=0x02; // Zero flag
        }
        else if (value & 0x8000)
        {
            registers.set_get_CC()=0x04; // Negative flag
        }
        else
        {
            registers.set_get_CC()=0x01; // Positive flag
        }
    }
        break;
    case 0xA:
        //LDI
    {
        uint16_t value = registers.set_get_MDR();
        // Update condition codes
        registers.setR(DR, value);
        if (value == 0)
        {
            uint16_t value = registers.set_get_MDR();
            // Update condition codes
            if (value == 0)
            {
                registers.set_get_CC()=0x02; // Zero flag
            }
            else if (value & 0x8000)
            {
                registers.set_get_CC()=0x04; // Negative flag
            }
            else
            {
                registers.set_get_CC()=0x01; // Positive flag
            }
        }
    }
        break;
    case 0x6:
        //LDR
    {
        uint16_t value = registers.set_get_MDR();
        // Update condition codes
        registers.setR(DR, value);
        if (value == 0)
        {
            uint16_t value = registers.set_get_MDR();
            // Update condition codes
            if (value == 0)
            {
                registers.set_get_CC()=0x02; // Zero flag
            }
            else if (value & 0x8000)
            {
                registers.set_get_CC()=0x04; // Negative flag
            }
            else
            {
                registers.set_get_CC()=0x01; // Positive flag
            }
        }
    }
        break;
    case 0xE:
        // LEA
    {
        // Store address in destination register
        registers.setR(DR, address);
    }
        break;
    case 0x3:
        //ST
    {
        // Set the value to be stored in MDR
        registers.set_get_MDR()=value;

        // Store the value in the DR to the computed address
        index = registers.set_get_MAR();
        memory.write(registers.set_get_MAR(), registers.set_get_MDR());
    }
        break;
    case 0xB:
        //STI
    {
        // Set the value to be stored in MDR
        registers.set_get_MDR()=value;
        // Store the value in the SR to the memory at the address pointed to by the computed address
        memory.write(memory.read(registers.set_get_MAR()), registers.set_get_MDR());
        // Store the value in the DR to the computed address
        index=memory.read(registers.set_get_MAR());
    }
        break;
    case 0x7:
        //STR
    {

        // Set the value to be stored in MDR
        registers.set_get_MDR()=value;

        // Store the value in the SR to the computed address
        index = registers.set_get_MAR();
        memory.write(registers.set_get_MAR(), registers.set_get_MDR());
    }
        break;
    case 0x4:
    {

        // JSR JSRR
        uint16_t currentPC = registers.set_get_PC(); // Get the current PC
        // Store the current PC in R7
        registers.setR(7, currentPC);
        registers.set_get_PC()=address; // Update PC with the offset
    }
        break;
    case 0x5:
        // AND
    {
        // Set condition codes
        registers.setR(DR, GateALU);
        uint16_t result = registers.getR(DR);
        if (result == 0)
        {
            registers.set_get_CC()=0x02;
        }
        else if (result >> 15)
        {
            registers.set_get_CC()=0x04; // Negative
        }
        else
        {
            registers.set_get_CC()=0x01; // Positive
        }
    }
        break;
    case 0xC:
        //RET JMP
    {

        registers.set_get_PC()=address;

    }
        break;
    case 0x9:
        // NOT
    {
        // Set condition codes
        registers.setR(DR, GateALU);
        uint16_t result = registers.getR(DR);
        if (result == 0)
        {
            registers.set_get_CC()=0x02; // Zero
        }
        else if (result >> 15)
        {
            registers.set_get_CC()=0x04; // Negative
        }
        else
        {
            registers.set_get_CC()=0x01; // Positive
        }
    }
        break;
    default:
        // Handle unsupported opcode
        break;
    }
}
bool LC3Cycle::isHalt(){
    return (registers.set_get_MDR() == 0xF025);
}

void LC3Cycle::ANDorADDdecode(){
    DR = (registers.set_get_IR() >> 9) & 0x7;       // Destination register
    SR1 = (registers.set_get_IR() >> 6) & 0x7;      // Source register 1
    is_imm = (registers.set_get_IR() >> 5) & 0x1; // Immediate flag

    if (is_imm)
    {
        imm5 = registers.set_get_IR() & 0x1F; // Extract immediate value
        // Sign-extend imm5 to 16 bits
        if (imm5 & 0x10) //Checking for negative value
        {
            imm5 |= 0xFFE0; // Sign extend to the left => => add 11 '1' digit to the left of the imm5
        }
    }
    else
    {
        SR2 = registers.set_get_IR() & 0x7; // Source register 2
    }
}
