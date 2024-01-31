/*************************************************************************
* Cpu.h
*
* This file contains the class definition for the CPU
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

#ifndef CPU_H
#define CPU_H
#include "DataMemory.h"
#include "InstructionMemory.h"
#include "RegisterFile.h"

class Cpu
{
    public:
        Cpu();
        void update(); // run the simulation
        void setDmem(unsigned int addr, unsigned int data);  // place a value in data memory
        void setImem(unsigned int addr, unsigned int data);  // place a value in instruction memory
        void dump(); // dump the cpu state to the standard output device
    private:
        InstructionMemory imem;
        DataMemory dmem;
        unsigned int pc;
        RegisterFile regs;
};

#endif // CPU_H
