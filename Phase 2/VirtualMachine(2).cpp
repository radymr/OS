/****************************************************************************
* VirtualMachine.cpp
* Ryan Rady
* Phase 1 - Assembler Program 
* Date 04.29.13
* Description: reads in a file from '.o' file via assembler program.
* reads instructions and executes commands
*****************************************************************************/  
#include "VirtualMachine.h"

using namespace std;

VirtualMachine::VirtualMachine() {
	// initialize everything.
	pc = 0, clock = 0, sp = 256, ir = 0, carry = 0, limit = 0; 
	statusreg.i = 0;
	statusreg.sr.carry = 0; 
	statusreg.sr.less = 0;
	statusreg.sr.equal = 0;
	statusreg.sr.greater = 0;
	reg = vector <int> (REG_FILE_SIZE); // initialize registers, (4)
	mem = vector <int> (MEM_SIZE);		// initialize  memory (256)

	for (int i = 0; i < 4; i++)
		reg[i] = 0;

	instrIN[0] = &VirtualMachine::load;
	instrIN[1] = &VirtualMachine::loadi;
	instrIN[2] = &VirtualMachine::store;
	instrIN[4] = &VirtualMachine::add;
	instrIN[5] = &VirtualMachine::addi;
	instrIN[6] = &VirtualMachine::addc;
	instrIN[7] = &VirtualMachine::addci;
	instrIN[8] = &VirtualMachine::sub;
	instrIN[9] = &VirtualMachine::subi;
	instrIN[10] = &VirtualMachine::subc;
	instrIN[11] = &VirtualMachine::subci;
	instrIN[12] = &VirtualMachine::And;
	instrIN[13] = &VirtualMachine::Andi;
	instrIN[14] = &VirtualMachine::Xor;
	instrIN[15] = &VirtualMachine::Xori;
	instrIN[16] = &VirtualMachine::Compl;
	instrIN[18] = &VirtualMachine::shl;
	instrIN[20] = &VirtualMachine::shla;
	instrIN[22] = &VirtualMachine::shr;
	instrIN[24] = &VirtualMachine::shra;
	instrIN[26] = &VirtualMachine::compr;
	instrIN[27] = &VirtualMachine::compri;
	instrIN[28] = &VirtualMachine::getstat;
	instrIN[30] = &VirtualMachine::putstat;
	instrIN[32] = &VirtualMachine::jump;
	instrIN[34] = &VirtualMachine::jumpl;
	instrIN[36] = &VirtualMachine::jumpe;
	instrIN[38] = &VirtualMachine::jumpg;
	instrIN[40] = &VirtualMachine::call;
	instrIN[42] = &VirtualMachine::Return;
	instrIN[44] = &VirtualMachine::read;
	instrIN[46] = &VirtualMachine::write;
	instrIN[48] = &VirtualMachine::halt;
	instrIN[50] = &VirtualMachine::noop; 

}//VirtualMachine Constructor

void VirtualMachine::LOAD(string infile){
    this->infile=infile;
    int i = 0; //for stack
	string ofile = infile.substr(0,infile.size()-2);
    ofile = ofile + ".o"; 
    fstream loadProg;
	loadProg.open(ofile.c_str(), ios::in);
	if (!loadProg.is_open()) {
		cout << ofile <<" failed to open.\n";
		exit(10);
		system("PAUSE");
	}

	getline(loadProg, line);
	while (!loadProg.eof()) {

		// the following line is the trick
		istringstream str(line.c_str());
		str >> objectcode;

		mem[i] = objectcode;//add program to top of stack
               
        instruction ins;
        ins.i = objectcode;
        OP=ins.f1.OP;
        I = ins.f1.I;
        opcode = (OP << 1) + I;
        i++;
		
		getline(loadProg, line);
                
        limit++;
	}//while
}//load

void VirtualMachine::FETCH(){
	while(pc != limit) {
		cout <<"Program counter = "<< pc << endl;	
		cout <<"Instruction Register = "<< ir << endl;
		ir = mem[pc];
		instruction ins;
		ins.i = ir;
		OP=ins.f1.OP;
		rd = ins.f1.RD;
		I = ins.f1.I;
		rs = ins.f1.RS;
		constant = ins.f3.CONST;
		address = ins.f2.ADDR;
		opcode = (OP << 1) + I;
		++pc;

		/*//for testing
		cout << "OP = " << OP << " Destination Register = " << rd << "I = " << I << endl;
		cout << "Source Register = " << rs << " Constant = " << constant << " Address = " << address << endl;
		cout <<"opcode = " << opcode <<endl;
		*/
		
		EXEC(opcode);

		if(mem[pc]==0){
			cout << "Error: Program out of bounds" << endl << endl;
            exit(11);
			system("PAUSE");
        }
	}//while
}//FETCH

void VirtualMachine::EXEC(int inst){
	(this->*instrIN[inst])();
}//EXEC

int VirtualMachine::checkNeg(int value){
	signed int mask = 0x8000; //mask for sign bit
	int negCheck = mask & value;
	if (negCheck == mask)
		value = value | 0xFFFF0000;
	return value; //returns 32 bit number
}//checkNeg

void VirtualMachine::checkCarry(int value) {
	int mask = 0x8000; 
    int carrycheck = mask & value;
	if(carrycheck == mask){
        statusreg.sr.carry = 1; 
    } else {
    	statusreg.sr.carry = 0; 
	}
}//checkCarry

void VirtualMachine::shiftCarryRight(int value) {
   	int carrycheck = value & 1; // mask is 1
   	if(carrycheck == 1){
       	statusreg.sr.carry = 1;
   	} else {
		statusreg.sr.carry = 0;
    }
}//shiftCarryRight
void VirtualMachine::shiftCarryLeft(int value) {
   	int mask = 0x8000;
    int carrycheck = mask & value;
	if(carrycheck == mask){
		statusreg.sr.carry = 1;
  	} else {
		statusreg.sr.carry = 0;
	}
}//shiftCarryLeft

void VirtualMachine::shiftRight(int value) {
	int mask = 0x8000; 
    int carrycheck = mask & value;
    if(carrycheck == mask){
		reg[rd] = reg[rd] | 0xFFFF0000;
    } else {
		reg[rd] = reg[rd] &0x0000FFFF;
	}
}//shiftRight

void VirtualMachine::checkOverflow(int value1,int value2) {
    int mask = 0x8000; 
    int temp1 = mask & value1;
    int temp2 = mask & value2;
    int sum = value1 + value2;
    sum = mask & sum;

    temp1 = temp1 >> 15;
    temp2 = temp2 >> 15;
    sum = sum >> 15;

    if ( temp1 == 1 && temp2 == 1 ) {
		if ( sum == 0 ) {
            statusreg.sr.overflow= 1;
            cout << "Error: An overflow has occured: " << endl;
            exit (12);
			system("PAUSE");
        }
    } else if ( temp1 == 0 && temp2 == 0 ) {
		if ( sum == 1 ) {
             statusreg.sr.overflow = 1;
             cout << "Error: An overflow has occured: "<< endl;
             exit (13);
			 system("PAUSE");
         }
   	} else {
         statusreg.sr.overflow = 0;
    }
}//checkOverflow

void VirtualMachine::checkLimit() {
	if(pc > limit){
		cout << "Error: Program counter exceeds limit" << endl;
		exit(14);
	}
}//checkLimit

void VirtualMachine::load() {
	reg[rd]=mem[address];
	cout << "load: " << "reg[" << rd << "]= " << reg[rd] <<" mem[" << address << "]= " << mem[address] << endl;
	clock += LSCRTICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::loadi() { 
	constant = checkNeg(constant);
	reg[rd] = constant;
	cout << "loadi: constant= " << constant << " reg[" << rd << "]= " << reg[rd] << endl;
	clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::store(){
	mem[address] = reg[rd];
	cout << "store: rd= " << rd << " address= " << address;
	cout << " mem[" << address << "]= " << mem[address] << endl;
	clock += LSCRTICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::add(){
	cout << "add: reg[" << rd << "]= " << reg[rd] << " + reg[" << rs << "]= " << reg[rs];
	checkOverflow(reg[rd],reg[rs]);
	reg[rd] = reg[rd] + reg[rs];
	cout << " = reg[rd]= " << reg[rd] << endl;
	checkCarry(reg[rd]);
	clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::addi(){
	constant = checkNeg(constant);
	cout << "addi: reg[rd]= " << reg[rd] << " + constant= " << constant;
	checkOverflow(reg[rd],constant);
	reg[rd] = reg[rd] + constant;
	cout << " = reg[rd]= " << reg[rd] << endl;
	checkCarry(reg[rd]);
	clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::addc(){
	checkOverflow(reg[rd],reg[rs]);
	cout << "addc: reg[" << rd << "]= " << reg[rd] << " + reg[" << rs << "]= " << reg[rs];
    reg[rd] = reg[rd] + reg[rs] + carry;
	cout << " + carry:" << carry << " = reg[rd]= " << reg[rd] << endl;
	checkCarry(reg[rd]); 
    clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::addci(){
	checkOverflow(reg[rd],constant);
	cout << "addci: reg[" << rd << "]= " << reg[rd] << " + constant= " << constant;
    reg[rd] = reg[rd] + constant + carry;
	cout << " + carry:" << carry << " = reg[" << rd << "]= " << reg[rd] << endl;
	checkCarry(reg[rd]); 
    clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::sub(){
    checkOverflow(reg[rd],reg[rs]);
	cout << "sub: reg[" << rd << "]= " << reg[rd] << " - reg[" << rs << "]= " << reg[rs] << " = ";
    reg[rd] = reg[rd] - reg[rs];
    clock += TICK;
    checkCarry(reg[rd]);
    cout << " reg[" << rd << "]= " << reg[rd] << endl;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::subi(){
    constant = checkNeg(constant);  
    checkOverflow(reg[rd],constant);  
	cout << "sub: reg[" << rd << "]= " << reg[rd] << " - constant= " << constant << " = ";
    reg[rd] = reg[rd] - constant;
    clock += TICK;
    checkCarry(reg[rd]);
    cout << " reg[" << rd << "]= " << reg[rd] << endl << " clock= " << clock << endl << endl;
}

void VirtualMachine::subc(){
	checkOverflow(reg[rd],reg[rs]);
	cout << "subc: reg[" << rd << "]= " << reg[rd] << " - reg[" << rs << "]= " << reg[rs];
    reg[rd] = reg[rd] - reg[rs] - carry;
	checkCarry(reg[rd]); 
	cout << " - carry:" << carry << " = reg[rd]= " << reg[rd] << endl;
    clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::subci(){
	checkOverflow(reg[rd],constant);
    cout << "subci: reg[" << rd << "]= " << reg[rd] << " - constant = " << constant;
    reg[rd] = reg[rd] - constant - carry;
	checkCarry(reg[rd]);
	cout << " - carry:" << carry << " = reg[" << rd << "]= " << reg[rd] << endl;
    clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::And(){
	cout << "And: reg[" << rd << "]= " << reg[rd] << " & reg[" << rs << "]= " << reg[rs] << " = ";
    reg[rd] = reg[rd] & reg[rs];
    clock += TICK;
	cout << "reg[" << rd << "]= " << reg[rd] << endl << " clock= " << clock << endl << endl;
}

void VirtualMachine::Andi(){
	constant = checkNeg(constant);
    cout << "Andi: reg[" << rd << "]= " << reg[rd] << " & constant = " << constant;
    reg[rd] = reg[rd] & constant;
    clock += TICK;
    cout << "= reg[" << rd << "]= " << reg[rd] << " clock= " << clock << endl << endl;
}

void VirtualMachine::Xor(){
	cout << "Xor: reg[" << rd << "]= " << reg[rd] << " ^ reg[" << rs << "]= " << reg[rs] << " = ";
    reg[rd] = reg[rd] ^ reg[rs];
    clock += TICK;
	cout << "reg[" << rd << "]= " << reg[rd] << endl << " clock= " << clock << endl << endl;
}

void VirtualMachine::Xori(){
	constant = checkNeg(constant);
	cout << "Xori: reg[" << rd << "]= " << reg[rd] << " & constant = " << constant;
    reg[rd] = reg[rd] ^ constant;
    clock += TICK;
    cout << "= reg[" << rd << "]= " << reg[rd] << " clock= " << clock << endl << endl;
}

void VirtualMachine::Compl(){
    reg[rd] = ~reg[rd];
	cout << "Compl: reg[" << rd << "]= " << reg[rd] << endl;
    clock += TICK;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::shl(){
    cout << "shl: reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
    shiftCarryLeft(reg[rd]);
    reg[rd] = reg[rd] << 1;
    reg[rd] = checkNeg(reg[rd]);
    clock += TICK;
    cout <<"post reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::shla(){
    cout << "shla: reg[ " << rd << "]= " << reg[rd] << " sr= " << statusreg.i;
    shl();
}

void VirtualMachine::shr(){
    cout << "shr: reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
    shiftCarryRight(reg[rd]);
    reg[rd] = reg[rd] >> 1;
    clock += TICK;
    cout << "post reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::shra(){
    cout << "shra: reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
    shiftCarryRight(reg[rd]);
    shiftRight(reg[rd]);
    reg[rd] = reg[rd] >> 1;
    clock += TICK;
    cout << "post reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::compr(){
    if(reg[rd] < reg[rs]){
		statusreg.sr.less = 1;
		statusreg.sr.equal = 0;
		statusreg.sr.greater = 0;
    }
    else if(reg[rd] == reg[rs]){
		statusreg.sr.less = 0;
		statusreg.sr.equal = 1;
		statusreg.sr.greater = 0;
    }
    else if(reg[rd] > reg[rs]){
		statusreg.sr.less = 0;
		statusreg.sr.equal = 0;
		statusreg.sr.greater = 1;
    }
    clock += TICK;
    cout << "compri: reg[" << rd << "]= " << reg[rd] << endl;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::compri(){
    if(reg[rd] < constant){
		statusreg.sr.less = 1;
		statusreg.sr.equal = 0;
		statusreg.sr.greater = 0;
    }
    else if(reg[rd] == constant){
		statusreg.sr.less = 0;
		statusreg.sr.equal = 1;
		statusreg.sr.greater = 0;
    }
    else if(reg[rd] > constant){
		statusreg.sr.less = 0;
		statusreg.sr.equal = 0;
		statusreg.sr.greater = 1;
    }
    clock += TICK;
    cout << "compri: reg[" << rd << "]= " << reg[rd] << endl;
	cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::getstat(){
    reg[rd] = statusreg.i;
    clock += TICK;
    cout << "getstat: sr= " << statusreg.i << " reg[" << rd << "]= " << rd << endl;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::putstat(){
    statusreg.i = reg[rd];
    clock += TICK; 
	cout << "putstat: sr= " << statusreg.i << " reg[" << rd << "]= " << rd << endl;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jump(){
    pc = address;
	checkLimit();
    clock += TICK;
    cout << "jump: address= " << address << " pc= " << pc << endl;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jumpl(){
    if(statusreg.sr.less==1){
		pc = address;
		checkLimit();
    }
    clock += TICK;
    cout << "jumpl: address= " << address << " pc= " << pc << endl;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jumpe(){
    if(statusreg.sr.equal==1){		   
		pc = address;
		checkLimit();
    }
    clock += TICK;
    cout << "jumpe: address= " << address << " pc= " << pc << endl;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jumpg(){
    if(statusreg.sr.greater==1){ 
		pc = address;
		checkLimit();
    }
    clock += TICK;
    cout << "jumpg: address= " << address << " pc= " << pc << endl;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::call(){
	cout << "call: sp= " << sp << " address= " << address << endl;
	sp = sp - 6;
	mem[sp+6] = pc;
	mem[sp+5] = reg[0];
	mem[sp+4] = reg[1];
	mem[sp+3] = reg[2];
	mem[sp+2] = reg[3];
	mem[sp+1] = statusreg.i;
	pc = address;
	clock += LSCRTICK;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::Return(){
	cout << "return: " << endl;
	sp = sp + 6;
	pc = mem[sp];
	reg[0] =  mem[sp-1];
	reg[1] =  mem[sp-2];
	reg[2] =  mem[sp-3];
	reg[3] =  mem[sp-4];
	statusreg.i  =  mem[sp-5];
	clock += LSCRTICK;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::read(){
	cout << "read: ";
	string ifile = infile.substr(0,infile.size()-1);
	ifile = ifile + "in"; 
	fstream inputfile;
	inputfile.open(ifile.c_str(), ios::in|ios::out);
	int content = 0;	// used to store content of .in file.
	if(inputfile.fail()){
		cout<<"Error opening file "<<infile<<",creating one with default value 0."<<endl;
		inputfile.open(ifile.c_str(), ios::out|ios::trunc);
		inputfile << content;
		inputfile.close();
		inputfile.open(ifile.c_str());
	}
    inputfile >> content;
    inputfile.close();
 
	cout << "rd= " << rd << " content= " << content << endl;
	reg[rd] = content;
	clock += RWTICK;
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::write(){
	cout <<"write: " << endl;
	string ofile = infile.substr(0,infile.size()-1);
	ofile = ofile + "out"; 
	ofstream outputfile;
	outputfile.open(ofile.c_str());
	if(outputfile.fail()){
		cout <<"Error when opening "<<infile<<"..."<<endl;
	} else {
		outputfile << reg[rd] << endl;
		outputfile.close();
	}
	clock += RWTICK;  
	cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::halt(){
	clock += TICK;
	cout << "halt: " << endl << " clock= " << clock << endl << endl;
	exit(15);
}
void VirtualMachine::noop(){

}
/***********************ERRORS******************************
* error(10) - (LOAD()) - File failed to open.
* error(11) - (FETCH()) - Program out of bounds
* error(12) - (checkOverflow) - An overflow has occured:
* error(13) - (checkOverflow) - An overflow has occured:
* error(14) - (checklimit) - Program counter exceeds limit
* error(15) - (halt()) - halt is exit command.
***********************************************************/
