/*************************************************************************
* Cpu.cpp
*
* This file contains the class implementation for the CPU
* for the single-cycle machine defined in "Computer Organization and
* Design, the Hardware Software Interface", Patterson & Hennessy, Fifth Edition.
*
* This code was developed as a demonstration for SER450 and is provided
* as-is as a learning aid for Chapter 4 of the class text.
*
* COPYRIGHT (C) 2019, Arizona State University
* ALL RIGHTS RESERVED
*
* Author: Doug Sandy
*
**************************************************************************/
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include "Cpu.h"
#include <iostream>

/*************************************************************************
* Student modifications should go here to give the proper values for these
* macro definitions



**************************************************************************/
// BITS(x, start, end) a macro function that takes three integer arguments
//    the output of the function will be bits start-end of of value x but
//    the bits in the output will be shifted so that bit start is at bit 
//    position 0.  Example: the input values 0x12345678, 8,12 will yield 
//    the result 0x00000016
#define BITS(x,start,end)      

// SIGN_EXT(x) a macro to sign extend a 16 bit signed integer to 32 bits
#define SIGN_EXT(x)            

// each of the following are the opcodes for teh specified instruction from
// the MIPS instruction set.
#define OP_LW       
#define OP_SW       
#define OP_RTYPE    
#define OP_BEQ      
#define OP_JMP

#define BITS(x, start, end) ((x >> start) & ((1 << (end - start + 1)) - 1))

#define SIGN_EXT(x) ((signed int)((x & 0x8000) ? (x | 0xFFFF0000) : x))

#define OP_LW 0x23
#define OP_SW 0x2B
#define OP_RTYPE 0x00
#define OP_BEQ 0x04
#define OP_JMP 0x02

//********************************************
// Constructor
// initialize the cpu object
Cpu::Cpu()
{
    // initialize the program counter to 0
    pc = 0;
}

//********************************************
// setImem
// set the value of a 32-bit word at a specified
// address in the CPU's instruction memory
void Cpu::setImem(unsigned int address,unsigned int value)
{
    imem.setAt(address,value);
}

//********************************************
// setImem
// set the value of a 32-bit word at a specified
// address in the CPU's data memory
void Cpu::setDmem(unsigned int address, unsigned int value)
{
    dmem.update(address, value, true);
}

//*************************************************
// update()
// This function simulates a single clock cycle of the 
// CPU.  This was covered in the class lecture notes
void Cpu::update()
{
    // calculate the combinational logic values - these become stable
    // some amount of time after the previous system clock.

    // fetch the current instruction
    unsigned int instruction = imem.value(pc);

    // extract the fields from the instruction
    unsigned int opcode = BITS(instruction, 26, 31);
    unsigned int funct = BITS(instruction, 0, 5);
    unsigned int rdidx = BITS(instruction, 11, 15);
    unsigned int rtidx = BITS(instruction, 16, 20);
    unsigned int rsidx = BITS(instruction, 21, 25);
    signed   int immed = SIGN_EXT(BITS(instruction, 0, 15));
    unsigned int jmpaddr = BITS(instruction, 0, 25);

    // alu operation combinational logic
    int ALUOp = 0x0;
    if ((opcode == 0x23) || (opcode == 0x2b)) ALUOp = 0x0;  // load or store instructions
    if (opcode == 0x04) ALUOp = 0x1;  // branch on equal instruction
    if (opcode == 0x00) ALUOp = 0x2;  // r-type instructions
    
    // alu control combinational logic
    int ALUControl = 0xF;
    if (ALUOp == 0x0) ALUControl = 0x2;  // lw/sw -> add
    if (ALUOp == 0x01) ALUControl = 0x6; // lw/sw -> subtract
    if (ALUOp == 0x02) {
        if (funct == 0x20) ALUControl = 0x2; // add
        if (funct == 0x22) ALUControl = 0x6; // subtract
        if (funct == 0x24) ALUControl = 0x0; // AND
        if (funct == 0x25) ALUControl = 0x1; // OR
        if (funct == 0x2a) ALUControl = 0x7; // set-on-less-than
    }

    // additional control signals based on opcode
    unsigned int aluSrc = (opcode == OP_LW) || (opcode == OP_SW) ? 1 : 0;
    unsigned int regDest = (opcode == OP_RTYPE) ? 1 : 0;
    unsigned int branch = (opcode == OP_BEQ) ? 1 : 0;
    unsigned int memRead = (opcode == OP_LW) ? 1 : 0;
    unsigned int memToReg = (opcode == OP_LW) ? 1 : 0;
    unsigned int memWrite = (opcode == OP_SW) ? 1: 0;
    unsigned int regWrite = (opcode == OP_LW) || (opcode == OP_RTYPE) ? 1 : 0;
    unsigned int jump = (opcode == OP_JMP) ? 1 : 0;

    // register file read operation based on rt and rd indicies
    unsigned int regRs = regs.readData1(rsidx);
    unsigned int regRt = regs.readData2(rtidx);

    // alu source selection multiplexor
    unsigned int operand1 = regRs;
    unsigned int operand2 = aluSrc ? immed : regRt;

    // ALU combinatinational logic
    int ALUResult = 0;
    if (ALUControl == 0x0) ALUResult = operand1 & operand2; // and
    if (ALUControl == 0x1) ALUResult = operand1 | operand2; // or
    if (ALUControl == 0x2) ALUResult = operand1 + operand2; // add
    if (ALUControl == 0x6) ALUResult = operand1 - operand2; // subtract
    if (ALUControl == 0x7) ALUResult = (operand1 < operand2) ? 1 : 0; // set less than
    if (ALUControl == 0xc) ALUResult = ~(operand1 | operand2); // nor
    int zero = (ALUResult == 0x00000000) ? 1 : 0;
    
    // read the data memory if required
    unsigned int memData = dmem.read( ALUResult, memRead);

    // register write data multipelexor
    unsigned int regWrData = memToReg ? memData : ALUResult;
    
    // register write destination multiplexor
    unsigned int regWrAddr = regDest ? rdidx : rtidx;

    // calculate the branch or jump address
    unsigned int branchAddr = ( immed << 2) + pc + 4;
    unsigned int fullJumpAddr = (( pc + 4 ) & 0xF0000000) | (jmpaddr << 2);
    unsigned int next_pc;

    // multiplexors to select the next program counter value
    next_pc = (branch && zero) ? branchAddr : pc + 4;
    next_pc = (jump) ? fullJumpAddr : next_pc;

    // update the cpu state based on the rising edge of the system clock
    // in real hardware, these updates would all happen simultaneously
    regs.update(regWrAddr,regWrData,regWrite);
    dmem.update(ALUResult,regRt,memWrite);
    pc = next_pc;

    /******************************************************************
    * STUDENT CODE
    * Instrument the CPU here to count the total number of instructions,
    * the total registesr write instructions, and the total memory
    * instructions
    ******************************************************************/
}

//**********************************************************************
// dump()
// dump the state of the CPU object to the standard output device
void Cpu::dump()
{
    printf("PROGRAM COUNTER = %08x   INSTRUCTION = %08x\n",pc, imem.value(pc));
    printf("REGISTER FILE\n");
    regs.dump();
    printf("\n");

    /*****************************************************************
    * STUDENT CODE
    * This would be a great place to output your instrumention results
    ******************************************************************/
}

// Function to read program instructions and data into memories
void readMemoryFromFile(const std::string &filename, Cpu &cpu)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Ignore comments (lines starting with '#')
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::istringstream iss(line);
        std::string addressStr, memTypeStr, dataStr;
        iss >> addressStr >> memTypeStr >> dataStr;

        // Convert hex strings to integers
        unsigned int address = std::stoul(addressStr, nullptr, 16);
        int memType = std::stoi(memTypeStr);
        unsigned int data = std::stoul(dataStr, nullptr, 16);

        // Check memory type and update the corresponding memory in the CPU
        if (memType == 1)
        {
            cpu.setImem(address, data);
        }
        else if (memType == 0)
        {
            cpu.setDmem(address, data);
        }
    }

    file.close();
}