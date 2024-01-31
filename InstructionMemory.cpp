/*************************************************************************
* InstructionMemory.cpp
*
* This file contains the class implementation for the instruction memory
* component of the single-cycle machine defined in "Computer Organization and
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
#include "InstructionMemory.h"

//*************************************************************************
// setAt()
// this function is not part of the single-cycle machine architecture, 
// but can be used by the CPU to load program code into instruction memory
void InstructionMemory::setAt(unsigned int addr, unsigned int value)
{
    addr = addr>>2;
    memory[addr]=value;
}

//*************************************************************************
// setAt()
// return the entire integer representation of the instruction at a 
// specified memory location
unsigned int InstructionMemory::value(unsigned int addr)
{
    addr = addr>>2;
    return memory[addr];
}
