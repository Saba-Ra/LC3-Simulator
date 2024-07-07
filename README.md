# LC3 Computer Architecture Simulation Project

This project implements a simulation of the LC3 computer architecture. The project consists of two main stages: assembling and simulation. Below is a detailed explanation of each stage and the steps involved.

## Project Overview

The LC3 simulation project involves reading assembly instructions from a file, processing these instructions, and then simulating the execution of these instructions in a simulated LC3 environment. The project consists of the following stages:

1. **Assembly Stage**:
2. **Simulation Stage**:

## Assembly Stage

### Step 1: Reading and Label Extraction

- The assembly file is read, and in the first pass, all labels and their corresponding addresses are identified and stored in a temp container.
  
### Step 2: Instruction Translation

- During the second pass, the exact addresses of the labels are known. Each line of the assembly file is then translated into binary code according to the instruction set and opcodes of the LC3 architecture.
- The translated binary code is then converted into the machine code and stored in a simulated memory array.
- The assembly process is completed when the `END` directive is encountered.
- The final memory content is written to a `.bin` file.

## Simulation Stage

### Step 3: Loading the Binary File

- The binary file created in the assembly stage is read, and the contents are loaded into the main memory of the simulated LC3 machine.

### Step 4: Instruction Cycle

The LC3 simulation involves executing instructions in a cycle consisting of six phases:

#### 1. FETCH
- The instruction at the address pointed to by the program counter (PC) is read from memory and loaded into the instruction register (IR).

#### 2. DECODE
- The opcode (the 4 most significant bits of the instruction) is used to identify the instruction type.
- The different components of the instruction (e.g., destination register, source registers, immediate values) are extracted and stored in the appropriate registers (e.g., DR, SR1, SR2).
- Sign extension is applied to fields such as PCoffset if necessary.

#### 3. EVALUATE ADDRESS
- Depending on the instruction, the effective address for memory operations is calculated.

#### 4. FETCH OPERANDS
- The operands required for the instruction are fetched from the appropriate registers or memory locations.

#### 5. EXECUTE
- The actual operation specified by the instruction is performed (e.g., addition, bitwise AND).

#### 6. STORE RESULT
- The result of the operation is stored in the appropriate destination register or memory location.

### Details of Each Phase

For detailed explanations of each phase, please refer to [this document](https://cs2461-2020.github.io/lectures/lc3ISA.pdf)
.

![Project Overview](https://github.com/Saba-Ra/LC3-Architecture/blob/68bb79b072e124666a59939fc3c4324cdb23ad81/Project%20Overview.png)

### How to Use:

First, click on the "Upload File" button. A dialog will open allowing you to select the input assembly file. After uploading the file, if the file extension is correct, you can click the "Start Assembly" button to convert the assembly file to binary and then load the memory with the correct values.

By clicking the "NEXT" button, the LC3 instruction cycle will be executed. You will be able to see the values of the registers and the contents of the memory cells. 

Finally, when the "HALT" instruction is reached, the program will end. :)


## Conclusion

This project demonstrates the key concepts of the LC3 computer architecture by simulating the assembly and execution of LC3 instructions. The implementation covers the entire process from reading and processing assembly instructions to executing them in a simulated LC3 environment.

For further details on the LC3 instruction set and execution phases, please refer to the [LC3 Architecture Manual](chrome-extension://efaidnbmnnnibpcajpcglclefindmkaj/https://www.cs.auckland.ac.nz/courses/compsci210s1c/lectures/Goodman/2013.03.28-1up.pdf).

