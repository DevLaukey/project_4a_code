/*************************************************************************
* RegisterFile.cpp
*
* This file contains the class implementation for the register file
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
#include <stdio.h>
#include "RegisterFile.h"

//************************************************************************
// RegisterFile()
// Constructor.  Initialize the register file
RegisterFile::RegisterFile()
{
    // set all register values to 0 initially
    for (unsigned int i=0;i<32;i++) regs[i]=0;
}

//********************************************************************
// update()
// write new data to the specified address if the write signal is true
void RegisterFile::update(unsigned int addr, unsigned int data, bool write)
{
    if (write) {
        regs[addr] = data;
    }
}

//********************************************************************
// readData1()
// return the value of the indexed register
unsigned int RegisterFile::readData1(unsigned int addr)
{
    return regs[addr];
}

//********************************************************************
// readData2()
// return the value of the indexed register
unsigned int RegisterFile::readData2(unsigned int addr)
{
    return regs[addr];
}

//********************************************************************
// dump()
// dump the conents of the register file to the standard output device
// in a human readable fashion.
void RegisterFile::dump(void)
{
    for (unsigned int i=0;i<8;i++) {
        printf("R%02x: %08x R%02x: %08x ",i,regs[i],i+8,regs[i+8]);
        printf("R%02x: %08x R%02x: %08x\n",i+16,regs[i+16],i+24,regs[i+24]);
    }
}
