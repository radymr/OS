/****************************************************************************
* Assembler.cpp
* Ryan Rady
* Phase 1 - Assembler Program 
* Date 04.29.13
* Description: reads in a file from argv via operating system program.
* Converts the opcode into strings of binary numbers.
* Outputs binary number opcode into '.o' file for Virtual Machine to read in.
*****************************************************************************/
#include "Assembler.h"

using namespace std;

//// Constructor ////
Assembler::Assembler()
{
	instr["load"] = &Assembler::load;
	instr["loadi"] = &Assembler::loadi;
	instr["store"] = &Assembler::store;
	instr["add"] = &Assembler::add;
	instr["addi"] = &Assembler::addi;
	instr["addc"] = &Assembler::addc;
	instr["addci"] = &Assembler::addci;
	instr["sub"] = &Assembler::sub;
	instr["subi"] = &Assembler::subi;
	instr["subc"] = &Assembler::subc;
	instr["subci"] = &Assembler::subci;
	instr["And"] = &Assembler::And;
	instr["Andi"] = &Assembler::Andi;
	instr["Xor"] = &Assembler::Xor;
	instr["Xori"] = &Assembler::Xori;
	instr["Compl"] = &Assembler::Compl;
	instr["shl"] = &Assembler::shl;
	instr["shla"] = &Assembler::shla;
	instr["shr"] = &Assembler::shr;
	instr["shra"] = &Assembler::shra;
	instr["compr"] = &Assembler::compr;
	instr["compri"] = &Assembler::compri;
	instr["getstat"] = &Assembler::getstat;
	instr["putstat"] = &Assembler::putstat;
	instr["jump"] = &Assembler::jump;
	instr["jumpl"] = &Assembler::jumpl;
	instr["jumpe"] = &Assembler::jumpe;
	instr["jumpg"] = &Assembler::jumpg;
	instr["call"] = &Assembler::call;
	instr["Return"] = &Assembler::Return;
	instr["read"] = &Assembler::read;
	instr["write"] = &Assembler::write;
	instr["halt"] = &Assembler::halt;
	instr["noop"] = &Assembler::noop; 
	cout << " -- Assembler Initiated --" << endl;
}

//// assemble ////
// opens file and checks opcodes for errors.
// infile is set by argv, program to be ran is set by command line.
void Assembler::assemble(string infile)
{
	//tries to open the input file, and verifies that its opened.
	this->infile = infile;
	cout << " File to be opened: " << infile << endl;
	fstream inProg;
	inProg.open(infile.c_str(), ios::in);
	//error check
	if (!inProg.is_open()) {
		cout << infile << " failed to open.\n";
		exit(1);
		//system("PAUSE"); // needed by Visual Studio 2010.
	} else {
		cout << " " << infile << " successfully opened.." << endl << endl;
	}	

	//changes file extension to ".o" file type. will be read by virtual machine.
	assembleOutFile = infile.substr(0, infile.size() - 2);
	assembleOutFile += ".o";

	//clears prog.o incase it already exists
	assembleOutStream.open(assembleOutFile.c_str(), ios::out | ios::trunc);
	assembleOutStream.close();

	//retrieves the data from input file.
	getline(inProg, line);

	//loop, retrieves data from inFile.
	while(!inProg.eof()) {
	cout << " Reading next instruction.. " << endl;

		//determines whether the incoming line is a comment.
		if(line[0] !='!') {
			istringstream str(line.c_str());
			try {
				str >> opcode >> rd >> temp;
				if (!instr[opcode])
					throw NullPointerException();
			}
			catch (NullPointerException e) {
			cerr << e.what() << endl;
			exit(5);
			}
		//input line was a comment.
		} else { 
			getline(inProg, line);
			continue;
		}

		cout << "  sending opcode: "<< opcode << " to Virtual Machine.." << endl;
		//executes opcode conversion, sends converted code to '.o' file to send to VM.
		(this->*instr[opcode])(opcode); 
		getline(inProg, line);

	}//while(!inProg.eof())

	inProg.close(); //close file.

}//assemble()

//creates output object file for virtual machine to read
void Assembler::assembleOut(int outbuffer){
	
	//open file to append new method to file.
	assembleOutStream.open(assembleOutFile.c_str(), ios::out | ios::app);

	//verifies file was opened
	if (!assembleOutStream.is_open()){
		cout << "output file failed to open.\n";
		exit(2);
		system("PAUSE");
	}

	//sends opcode info to '.o' file.
	if(!assembleOutStream.eof())
		assembleOutStream << outbuffer << endl;

	//close file.
	assembleOutStream.close();
}

//checks the sign of the number, returns 
int Assembler::checkNegative(int checkNegIn){
	int tempNeg = checkNegIn;
	//used to determine 8 bits of negative number.
	unsigned int checkNegMask = 0xFFFFFF00;
	int negCheck = checkNegMask & checkNegIn;

	if(negCheck == checkNegMask)
		tempNeg = tempNeg & 255;

	return tempNeg;
}

//verifies register is in range. 
void Assembler::checkRegister(){
	if (rd > 3 || rd < 0){
		cout << "Register RD out of range " << endl;
		exit(3);
		system("PAUSE");
	}
	if (rs > 3 || rs < 0){
		cout << "Register RS out of range " << endl;
		exit(4);
		system("PAUSE");
	}
}

//opcode Map conversions.
void Assembler::load(string str){
	rs = 0;
	checkRegister();
	address = temp;
	int outbuffer = 0, load = 0, i = 0;
	//send opcode to '.o' file to be read by VM
	outbuffer = (load << 11)+(rd << 9)+(i << 8) + address; 
	assembleOut(outbuffer);
}

void Assembler::loadi(string str){  
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, loadi = 0, i = 1;
	outbuffer = (loadi << 11)+(rd << 9)+(i << 8)+ constant; 
	assembleOut(outbuffer); 
}

void Assembler::store(string str){
	rs = 0;
	checkRegister();
	address = temp;
	int outbuffer = 0, store = 1, i = 0;
	outbuffer = (store << 11)+(rd << 9)+(i << 8)+ address; 
	assembleOut(outbuffer);	
}

void Assembler::add(string str){
	rs = temp;
	checkRegister();
	int outbuffer = 0, add = 2, i = 0;
	outbuffer = (add << 11)+(rd << 9)+(i << 8)+(rs << 6);
	assembleOut(outbuffer);
}

void Assembler::addi(string str){
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, addi = 2, i = 1;
	outbuffer = (addi << 11)+(rd << 9)+(i << 8)+ constant; 
	assembleOut(outbuffer);
}

void Assembler::addc(string str){
	rs = temp;
	checkRegister();
	int outbuffer = 0, addc = 3, i = 0;
	outbuffer = (addc << 11)+(rd << 9)+(i << 8)+(rs << 6); 
	assembleOut(outbuffer);
}

void Assembler::addci(string str){ 
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, addci = 3, i = 1;
	outbuffer = (addci << 11)+(rd << 9)+(i << 8)+ constant; 
	assembleOut(outbuffer);
}

void Assembler::sub(string str){
	rs = temp;
	checkRegister();
	int outbuffer = 0, sub = 4, i = 0;
	outbuffer = (sub << 11)+(rd << 9)+(i << 8)+(rs << 6); 
	assembleOut(outbuffer);
}

void Assembler::subi(string str){
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, subi = 4, i = 1;
	outbuffer = (subi << 11)+(rd << 9)+(i << 8)+ constant; 
	assembleOut(outbuffer);
}

void Assembler::subc(string str){
	rs = temp;
	checkRegister();
	int outbuffer = 0, subc = 5, i = 0;
	outbuffer = (subc << 11)+(rd << 9)+(i << 8)+(rs << 6); 
	assembleOut(outbuffer);
}

void Assembler::subci(string str){
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, subci = 5, i = 1;
	outbuffer = (subci << 11)+(rd << 9)+(i << 8)+ constant;
	assembleOut(outbuffer);
}

void Assembler::And(string str){  
	rs = temp;
	checkRegister();
	int outbuffer = 0, And = 6, i = 0;
	outbuffer = (And << 11)+(rd << 9)+(i << 8)+(rs << 6); 
	assembleOut(outbuffer);
}

void Assembler::Andi(string str){
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, Andi = 6, i = 1;
	outbuffer = (Andi << 11)+(rd << 9)+(i << 8)+ constant; 
	assembleOut(outbuffer);
}

void Assembler::Xor(string str){  
	rs = temp;
	checkRegister();
	int outbuffer = 0, Xor = 7, i = 0;
	outbuffer = (Xor << 11)+(rd << 9)+(i << 8)+(rs << 6); 
	assembleOut(outbuffer);
}

void Assembler::Xori(string str){
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, Xori = 6, i = 1;
	outbuffer = (Xori << 11)+(rd << 9)+(i << 8)+ constant; 
	assembleOut(outbuffer);
}

void Assembler::Compl(string str){
	rs = 0;
	checkRegister();
	int outbuffer = 0, Compl = 8, i = 0;
	outbuffer = (Compl << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::shl(string str){
	rs = 0;
	checkRegister();
	int outbuffer = 0, shl = 9,  i = 0;
	outbuffer = (shl << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::shla(string str){
	rs = 0;
	checkRegister();
	int outbuffer = 0, shla = 10, i = 0;
	outbuffer = (shla << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::shr(string str){
	rs = 0;
	checkRegister();
	int outbuffer = 0, shr = 11, i = 0;
	outbuffer = (shr << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::shra(string str){
	rs = 0;
	checkRegister();
	int outbuffer = 0, shra = 12, i = 0;
	outbuffer = (shra << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::compr(string str){
	rs = temp;
	checkRegister();
	int outbuffer = 0, compr = 13, i = 0;
	outbuffer = (compr << 11)+(rd << 9)+(i << 8)+(rs << 6); 
	assembleOut(outbuffer);
}

void Assembler::compri(string str){
	rs = 0;
	checkRegister();
	constant = temp;
	constant = checkNegative(constant);
	int outbuffer = 0, compri = 13, i = 1;
	outbuffer = (compri << 11)+(rd << 9)+(i << 8)+constant; 
	assembleOut(outbuffer);
}

void Assembler::getstat(string str){	  
	rs = 0;
	checkRegister();
	int outbuffer = 0, getstat = 14, i = 0;
	outbuffer = (getstat << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::putstat(string str){	  
	rs = 0;
	checkRegister();
	int outbuffer = 0, putstat = 15, i = 0;
	outbuffer = (putstat << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::jump(string str){	  
	address = rd;
	rd = 0;
	int outbuffer = 0, jump = 16, i = 0;
	outbuffer = (jump << 11)+(rd << 9)+(i << 8)+address; 
	assembleOut(outbuffer);
}

void Assembler::jumpl(string str){
	address = rd;
	rd = 0;
	int outbuffer = 0, jumpl = 17,  i = 0;
	outbuffer = (jumpl << 11)+(rd << 9)+(i << 8)+address; 
	assembleOut(outbuffer);
}

void Assembler::jumpe(string str){
	address = rd;
	rd = 0;
	int outbuffer = 0, jumpe = 18, i = 0;
	outbuffer = (jumpe << 11)+(rd << 9)+(i << 8)+address; 
	assembleOut(outbuffer);
}

void Assembler::jumpg(string str){
	address = rd;
	rd = 0;
	int outbuffer = 0, jumpg = 19, i = 0;
	outbuffer = (jumpg << 11)+(rd << 9)+(i << 8)+address; 
	assembleOut(outbuffer);
}

void Assembler::call(string str){
	address = rd;
	rd = 0;
	int outbuffer = 0, call = 20, i = 0;
	outbuffer = (call << 11)+(rd << 9)+(i << 8)+address; 
	assembleOut(outbuffer);
}

void Assembler::Return(string str){
	rd = 0;
	int outbuffer = 0, Return = 21, i = 0;
	outbuffer = (Return << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::read(string str){
	rs = 0;
	checkRegister();
	int outbuffer = 0, read = 22, i = 0;
	outbuffer = (read << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::write(string str){
	rs = 0;
	checkRegister();
	int outbuffer = 0, write = 23, i = 0;
	outbuffer = (write << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::halt(string str){
	rs = 0;
	rd = 0;
	int outbuffer = 0, halt = 24, i = 0;
	outbuffer = (halt << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

void Assembler::noop(string str){
	rs = 0;
	rd = 0;
	int outbuffer = 0, noop = 25, i = 0;
	outbuffer = (noop << 11)+(rd << 9)+(i << 8); 
	assembleOut(outbuffer);
}

/**********************ERRORS*******************************
* error(1) - (assemble) - File failed to open.
* error(2) - (assembleOut) - output file failed to open .
* error(3) - (checkRediger) - RD is out of range.
* error(4) - (checkRediger) - RS is out of range.
* error(5) - (assemble) - NullPointerException
***********************************************************/
