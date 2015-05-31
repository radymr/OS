/****************************************************************************
* VirtualMachine.h
* Ryan Rady
* Phase 1 - Assembler Program 
* Date 04.29.13
* Description: reads in a file from '.o' file via assembler program.
* reads instructions and executes commands
*****************************************************************************/        
#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H
#include "Assembler.h"
#include <iostream> // for cout
#include <string>	// String
#include <map>		// Map
#include <vector>	// Vector
#include <stdexcept>// for error check.
#include <cstdlib>  // for exit()
#include <fstream>  // for fstream::open()
#include <sstream>  // for istringstream

using namespace std;

//opcode sent to each of these formats
//to put the correct bits into the correct variables
//based on which format is needed for certain opcodes.

class format0{
public:
	unsigned carry:1;
	unsigned greater:1;
	unsigned equal:1;
	unsigned less:1;
    unsigned overflow:1;
	unsigned VM_RETURN_STATUS:3;
	unsigned IO_REGISTER:2;
};

class format1 {
public:
	unsigned UNUSED:6;
	unsigned RS:2;
	unsigned I:1;
	unsigned RD:2;
	unsigned OP:5;
};

class format2 {
public:
	unsigned ADDR:8;
	unsigned I:1;
	unsigned RD:2;
	unsigned OP:5;
};

class format3 {
public:
	int CONST:8;
	unsigned I:1;
	unsigned RD:2;
	unsigned OP:5;
};

union instruction {
	int i;
	format1 f1;
	format2 f2;
	format3 f3;
};

 union statusRegister{
	int i;
    format0 sr;
};

class VirtualMachine{
public:
	typedef void (VirtualMachine::*FP)();
	//constructor
	VirtualMachine();
	//read opcode from .o file and stores object codes to top of stack
	void LOAD(string infile);	
	//converts code to necessary format, and sends code to EXEC.
	void FETCH();
	//Executes code.
	void EXEC(int inst);
	//Check very important information
	int checkNeg(int value);
	void checkCarry(int value);
	void shiftCarryRight(int value);
	void shiftCarryLeft(int value);
	void shiftRight(int value);
	void checkOverflow(int value1,int value2);
	void checkLimit();
	//map
	void load();
	void loadi();
	void store();
	void add();
	void addi();
	void addc();
	void addci();
	void sub();
	void subi();
	void subc();
	void subci();
	void And();
	void Andi();
	void Xor();
	void Xori();
	void Compl();
	void shl();
	void shla();
	void shr();
	void shra();
	void compr();
	void compri();
	void getstat();
	void putstat();
	void jump();
	void jumpl();
	void jumpe();
	void jumpg();
	void call();
	void Return();
	void read();
	void write();
	void halt();
	void noop();
private:
	statusRegister statusreg;
	string infile, line;
	static const int REG_FILE_SIZE = 4;
	static const int MEM_SIZE = 256;
	vector <int> reg, mem;
	vector <string> jobList;
	map <int,FP> instrIN;
	unsigned int address;
	int sp, pc, ir, carry, limit, lessThan, equal, greaterThan;
	int rd, rs, constant, clock, objectcode, OP, I, opcode, timeSlice, begin;
	//clock
	const static int TICK = 1;    //generic clock tick
	const static int RWTICK = 28; //read and write clock tick
	const static int LSCRTICK = 4;//load, store, call, and return clock tick
	const static int CSTICK = 10; //Context Switch Tick
};//VirtualMachine
#endif
