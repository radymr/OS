/****************************************************************************
* Assembler.cpp
* Ryan Rady
* Phase 2 - Operating System
* Date 05.13.13
* Description: reads in a file from argv via operating system program.
* Converts the opcode into strings of binary numbers.
* Outputs binary number opcode into '.o' file for Virtual Machine to read in.
*****************************************************************************/
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <iostream> // for cout, ...
#include <string>	// String
#include <map>		// Map
#include <stdexcept>// for error check.
#include <cstdlib>  // for exit()
#include <fstream>  // for fstream::open()
#include <sstream>  // for istringstream

using namespace std;

// For error checking.
class NullPointerException: public runtime_error {
public:
	NullPointerException(): runtime_error("Null Pointer!") { }
};

class Assembler{
public:
	// define Function Pointer type
	typedef void (Assembler::*FP)(string);
	
	// each instruction has its own function
	void load(string s);
	void loadi(string s);
	void store(string s);
	void add(string s);
	void addi(string s);
	void addc(string s);
	void addci(string s);
	void sub(string s);
	void subi(string s);
	void subc(string s);
	void subci(string s);
	void And(string s);
	void Andi(string s);
	void Xor(string s);
	void Xori(string s);
	void Compl(string s);
	void shl(string s);
	void shla(string s);
	void shr(string s);
	void shra(string s);
	void compr(string s);
	void compri(string s);
	void getstat(string s);
	void putstat(string s);
	void jump(string s);
	void jumpl(string s);
	void jumpe(string s);
	void jumpg(string s);
	void call(string s);
	void Return(string s);
	void read(string s);
	void write(string s);
	void halt(string s);
	void noop(string s);

	// default constructor
	Assembler(); 
	void assemble(string infile);
	//used to output object file.
	void assembleOut(int outbuffer);
	//checks the sign of the number
	int checkNegative(int checkNegIn);
	//verifies register is in range.
	void checkRegister();
	/*used in assemble() then in assembleOut() as a holder
	for "program name".o file to be read by virtual machine. */
	string assembleOutFile;
private:
	// the map
	map<string, FP> instr;
	//register destination, register source, contant, and temp holder.
	int rd, rs, constant, temp;
	//memory address used by opcode.
	unsigned int address;
	//for "program name".o file to be read by virtual machine.
	fstream inProg, assembleOutStream;
	//"program name".s or incoming file, line gathered from file, opcode to convert.
	string infile, line, opcode;
};// end assembler

#endif
