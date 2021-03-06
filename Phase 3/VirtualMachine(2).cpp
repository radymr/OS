/****************************************************************************
* VirtualMachine.cpp
* Ryan Rady
* Phase 2 - Operating System
* Date 05.13.13
* Description: reads in a file from '.o' file via assembler program.
* reads instructions and executes commands
*****************************************************************************/  
#include "VirtualMachine.h"

using namespace std;

VirtualMachine::VirtualMachine() {
	// initialize everything.
	pc = 0, clock = 0, sp = 256, ir = 0, carry = 0, limit = 0, base = 0; 
	statusreg.i = 0;
	statusreg.sr.carry = 0; 
	statusreg.sr.less = 0;
	statusreg.sr.equal = 0;
	statusreg.sr.greater = 0;
	begin = 0;
	reg = vector <int> (REG_FILE_SIZE); // initialize registers, (4)
	mem = vector <int> (MEM_SIZE);	// initialize  memory (256)
	pageTable = vector <int> (32);
	freeFrameList = vector <int> (32);

	for (int i = 0; i < 4; i++)
		reg[i] = 0;
	for (int i = 0; i < 32; i ++){
		pageTable[i] = 0;
		freeFrameList[i] = 0;
	}

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
	//output to log.txt
	fstream logOut;
	logOut.open("log.txt", fstream::in | fstream::out | ios::app);
	logOut << " -- Virtual Machine Initialized -- " << endl;
	logOut.close();

}//VirtualMachine Constructor

void VirtualMachine::LOAD(string infile){
    this->infile=infile;
	int frame = 0;
	for (int j = 0; j < 32; j++){
		if (freeFrameList[j] == 0){
			cout << "J: " << j << " " << freeFrameList[j] << endl;
			frame = j;
			freeFrameList[j] = 1;
			break;
		}
	}
	
	base = frame * 8;
	limit = base + 8;
	int i = base;
	pc = frame * 8;

	for (int i = base; i < limit; i++)
		mem[i] = 0;	

	string ofile = infile.substr(0,infile.size()-2);
	cout << ofile << " test" << endl;
    ofile = ofile + ".o"; 
    fstream loadProg;
	loadProg.open(ofile.c_str(), ios::in);
	if (!loadProg.is_open()) {
		cout << ofile <<" failed to open.\n";
		exit(10);
		system("PAUSE");
	}
	cout << "Test1" << endl;
	string trash;
	/*if something something..	
	if (pc > 0){
		cout << pc << endl;
		for (int k = 0; k == pc; k++){	
			getline(loadProg, trash);
			cout << "trash: " << trash << endl;
		}
		cin >> trash;
	}*/

	getline(loadProg, line);
	while (!loadProg.eof()) {

		// the following line is the trick
		istringstream str(line.c_str());
		str >> objectcode;
			//output to log.txt
			fstream logOut;
			logOut.open("log.txt", fstream::in | fstream::out | ios::app);
			logOut << ofile << endl;
			logOut << "  mem"<<i<<":"<<objectcode << endl;
			logOut.close();
			
		mem[i] = objectcode;//add program to top of stack

        instruction ins;
        ins.i = objectcode;
        OP=ins.f1.OP;
        I = ins.f1.I;
        opcode = (OP << 1) + I;

		if (i == limit -1)
			break;
        i++;
		
		getline(loadProg, line);
	}//while
}//load

void VirtualMachine::FETCH(){
	if(pc == limit){
		cout << "Page Fault!" << endl;
		fstream logOut;
		logOut.open("log.txt", fstream::in | fstream::out | ios::app);
		statusreg.sr.PAGE_FAULT_STATUS = 1;
		logOut << "  ** Page Fault! ** " << endl;
		logOut.close();
		//exit(1);
	}
	while(pc != limit) {
		//cout <<"Program counter = "<< pc << endl;	
		//cout <<"Instruction Register = "<< ir << endl;

		//output to log.txt
		fstream logOut;
		logOut.open("log.txt", fstream::in | fstream::out | ios::app);
		logOut << " Program counter = "<< pc << endl;
		logOut << " Instruction Register = "<< ir << endl<<endl;
		

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
		statusreg.sr.PAGE_FAULT_STATUS = 0;
		++pc;

		
		//for testing
		logOut << this->infile << " OP = " << OP << " Destination Register = " << rd << " I = " << I << endl;
		logOut << "Source Register = " << rs << " Constant = " << constant << " Address = " << address << endl;
		logOut <<"opcode = " << opcode <<endl<<endl<<endl;
		logOut.close();
		
		EXEC(opcode);

		if(mem[pc] == 0) {
			//cout << "Error: Program out of bounds" << endl << endl;
			//the OS will have the ability to handle this situation.
			break;
            //exit(11);
			//system("PAUSE");
        }
		if(statusreg.sr.VM_RETURN_STATUS == 1) { 
			break;
		}
		else if((statusreg.sr.VM_RETURN_STATUS == 6)||(statusreg.sr.VM_RETURN_STATUS == 7)) {	
			break;	
		}
        else if(timeSlice >= 14) {
			statusreg.sr.VM_RETURN_STATUS = 0;	 
			break;
        }
		else if(pc == limit) {
			break;
		}
		//cout <<"Time slice: "<<timeSlice<<endl;
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
			//system("PAUSE");
        }
    } else if ( temp1 == 0 && temp2 == 0 ) {
		if ( sum == 1 ) {
             statusreg.sr.overflow = 1;
             cout << "Error: An overflow has occured: "<< endl;
             exit (13);
			 //system("PAUSE");
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
	reg[rd]=mem[address + base];
	//cout << "load: " << "reg[" << rd << "]= " << reg[rd] <<" mem[" << address + base<< "]= " << mem[address + base] << endl;
	clock += LSCRTICK;
	timeSlice += LSCRTICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::loadi() { 
	constant = checkNeg(constant);
	reg[rd] = constant;
	//cout << "loadi: constant= " << constant << " reg[" << rd << "]= " << reg[rd] << endl;
	clock += TICK;
	timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::store(){
	mem[address + base] = reg[rd];
	//cout << "store: rd= " << rd << " address= " << address + base;
	//cout << " mem[" << address + base << "]= " << mem[address + base] << endl;
	clock += LSCRTICK;
	timeSlice += LSCRTICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::add(){
	//cout << "add: reg[" << rd << "]= " << reg[rd] << " + reg[" << rs << "]= " << reg[rs];
	checkOverflow(reg[rd],reg[rs]);
	reg[rd] = reg[rd] + reg[rs];
	//cout << " = reg[rd]= " << reg[rd] << endl;
	checkCarry(reg[rd]);
	clock += TICK;
	timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::addi(){
	constant = checkNeg(constant);
	//cout << "addi: reg[rd]= " << reg[rd] << " + constant= " << constant;
	checkOverflow(reg[rd],constant);
	reg[rd] = reg[rd] + constant;
	//cout << " = reg[rd]= " << reg[rd] << endl;
	checkCarry(reg[rd]);
	clock += TICK;
	timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::addc(){
	checkOverflow(reg[rd],reg[rs]);
	//cout << "addc: reg[" << rd << "]= " << reg[rd] << " + reg[" << rs << "]= " << reg[rs];
    reg[rd] = reg[rd] + reg[rs] + carry;
	//cout << " + carry:" << carry << " = reg[rd]= " << reg[rd] << endl;
	checkCarry(reg[rd]); 
    clock += TICK;
    timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::addci(){
	checkOverflow(reg[rd],constant);
	//cout << "addci: reg[" << rd << "]= " << reg[rd] << " + constant= " << constant;
    reg[rd] = reg[rd] + constant + carry;
	//cout << " + carry:" << carry << " = reg[" << rd << "]= " << reg[rd] << endl;
	checkCarry(reg[rd]); 
    clock += TICK;
    timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::sub(){
    checkOverflow(reg[rd],reg[rs]);
	//cout << "sub: reg[" << rd << "]= " << reg[rd] << " - reg[" << rs << "]= " << reg[rs] << " = ";
    reg[rd] = reg[rd] - reg[rs];
    clock += TICK;
    timeSlice += TICK;
    checkCarry(reg[rd]);
    //cout << " reg[" << rd << "]= " << reg[rd] << endl;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::subi(){
    constant = checkNeg(constant);  
    checkOverflow(reg[rd],constant);  
	//cout << "sub: reg[" << rd << "]= " << reg[rd] << " - constant= " << constant << " = ";
    reg[rd] = reg[rd] - constant;
    clock += TICK;
    timeSlice += TICK;
    checkCarry(reg[rd]);
    //cout << " reg[" << rd << "]= " << reg[rd] << endl << " clock= " << clock << endl << endl;
}

void VirtualMachine::subc(){
	checkOverflow(reg[rd],reg[rs]);
	//cout << "subc: reg[" << rd << "]= " << reg[rd] << " - reg[" << rs << "]= " << reg[rs];
    reg[rd] = reg[rd] - reg[rs] - carry;
	checkCarry(reg[rd]); 
	//cout << " - carry:" << carry << " = reg[rd]= " << reg[rd] << endl;
    clock += TICK;
    timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::subci(){
	checkOverflow(reg[rd],constant);
    //cout << "subci: reg[" << rd << "]= " << reg[rd] << " - constant = " << constant;
    reg[rd] = reg[rd] - constant - carry;
	checkCarry(reg[rd]);
	//cout << " - carry:" << carry << " = reg[" << rd << "]= " << reg[rd] << endl;
    clock += TICK;
    timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::And(){
	//cout << "And: reg[" << rd << "]= " << reg[rd] << " & reg[" << rs << "]= " << reg[rs] << " = ";
    reg[rd] = reg[rd] & reg[rs];
    clock += TICK;
    timeSlice += TICK;
	//cout << "reg[" << rd << "]= " << reg[rd] << endl << " clock= " << clock << endl << endl;
}

void VirtualMachine::Andi(){
	constant = checkNeg(constant);
    //cout << "Andi: reg[" << rd << "]= " << reg[rd] << " & constant = " << constant;
    reg[rd] = reg[rd] & constant;
    clock += TICK;
    timeSlice += TICK;
    //cout << "= reg[" << rd << "]= " << reg[rd] << " clock= " << clock << endl << endl;
}

void VirtualMachine::Xor(){
	//cout << "Xor: reg[" << rd << "]= " << reg[rd] << " ^ reg[" << rs << "]= " << reg[rs] << " = ";
    reg[rd] = reg[rd] ^ reg[rs];
    clock += TICK;
    timeSlice += TICK;
	//cout << "reg[" << rd << "]= " << reg[rd] << endl << " clock= " << clock << endl << endl;
}

void VirtualMachine::Xori(){
	constant = checkNeg(constant);
	//cout << "Xori: reg[" << rd << "]= " << reg[rd] << " & constant = " << constant;
    reg[rd] = reg[rd] ^ constant;
    clock += TICK;
    timeSlice += TICK;
    //cout << "= reg[" << rd << "]= " << reg[rd] << " clock= " << clock << endl << endl;
}

void VirtualMachine::Compl(){
    reg[rd] = ~reg[rd];
	//cout << "Compl: reg[" << rd << "]= " << reg[rd] << endl;
    clock += TICK;
    timeSlice += TICK;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::shl(){
    //cout << "shl: reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
    shiftCarryLeft(reg[rd]);
    reg[rd] = reg[rd] << 1;
    reg[rd] = checkNeg(reg[rd]);
    clock += TICK;
    timeSlice += TICK;
    //cout <<"post reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
	//cout << " clock= " << clock << endl << endl;
}

void VirtualMachine::shla(){
    //cout << "shla: reg[ " << rd << "]= " << reg[rd] << " sr= " << statusreg.i;
    shl();
	clock += TICK;
	timeSlice += TICK;
}

void VirtualMachine::shr(){
    //cout << "shr: reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
    shiftCarryRight(reg[rd]);
    reg[rd] = reg[rd] >> 1;
    clock += TICK;
    timeSlice += TICK;
    //cout << "post reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::shra(){
    //cout << "shra: reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
    shiftCarryRight(reg[rd]);
    shiftRight(reg[rd]);
    reg[rd] = reg[rd] >> 1;
    clock += TICK;
    timeSlice += TICK;
    //cout << "post reg[" << rd << "]= " << reg[rd] << " sr= " << statusreg.i << endl;
	//cout << " clock= " << clock << endl << endl;
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
    timeSlice += TICK;
}

void VirtualMachine::compri(){
	//cout << reg[rd] << " " << constant << endl;
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
    timeSlice += TICK;
}

void VirtualMachine::getstat(){
    reg[rd] = statusreg.i;
    clock += TICK;
    timeSlice += TICK;
    //cout << "getstat: sr= " << statusreg.i << " reg[" << rd << "]= " << rd << endl;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::putstat(){
    statusreg.i = reg[rd];
    clock += TICK; 
    timeSlice += TICK; 
	//cout << "putstat: sr= " << statusreg.i << " reg[" << rd << "]= " << rd << endl;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jump(){
    pc = address + base;
	//checkLimit();
    clock += TICK;
    timeSlice += TICK;
    //cout << "jump: address= " << address + base << " pc= " << pc << endl;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jumpl(){
    if(statusreg.sr.less==1){
		pc = address + base;
		//checkLimit();
    }
    clock += TICK;
    timeSlice += TICK;
    //cout << "jumpl: address= " << address + base << " pc= " << pc << endl;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jumpe(){
    if(statusreg.sr.equal==1){		   
		pc = address + base;
		//checkLimit();
    }
    clock += TICK;
    timeSlice += TICK;
    //cout << "jumpe: address= " << address + base << " pc= " << pc << endl;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::jumpg(){
    if(statusreg.sr.greater==1){ 
		pc = address + base;
		//checkLimit();
    }
    clock += TICK;
    timeSlice += TICK;
    //cout << "jumpg: address= " << address + base << " pc= " << pc << endl;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::call(){
	sp = sp - 6;
	mem[sp+6] = pc;
	mem[sp+5] = reg[0];
	mem[sp+4] = reg[1];
	mem[sp+3] = reg[2];
	mem[sp+2] = reg[3];
	mem[sp+1] = statusreg.i;
	pc = address + base;
	clock += LSCRTICK;
	timeSlice += LSCRTICK;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::Return(){
	sp = sp + 6;
	pc = mem[sp];
	reg[0] =  mem[sp-1];
	reg[1] =  mem[sp-2];
	reg[2] =  mem[sp-3];
	reg[3] =  mem[sp-4];
	statusreg.i  =  mem[sp-5];
	clock += LSCRTICK;
	timeSlice += LSCRTICK;
	//cout << "  clock= " << clock << endl << endl;
}

void VirtualMachine::read(){
	string ifile = infile.substr(0,infile.size()-1);
	ifile = ifile + "in"; 
	fstream inputfile;
	inputfile.open(ifile.c_str(), ios::out|ios::in);
	int content;	// used to store content of .in file.

	//if (input is not a ' ')
		//inputfile >> content;
		//vector.puch_back(content);
	//else getline ' '
		//retrieve the rest of the vector.
		//content = 0
		//exit(1) // error

	//content = first element of vector.

	//while (not end of vector)
		//rest of vector output into file
		//output a space
		//output content.
			
    inputfile >> content;
	//cout << "Content: " << content << endl;
    //inputfile.close();
 
	reg[rd] = content;
	clock += RWTICK;
	timeSlice += RWTICK;
    statusreg.sr.IO_REGISTER = rd;	
    statusreg.sr.VM_RETURN_STATUS = 6;	
}

void VirtualMachine::write(){
	string ofile = infile.substr(0,infile.size()-1);
	ofile = ofile + "out"; 
	ofstream outputfile;
	outputfile.open(ofile.c_str());
	if(outputfile.fail()){
		cout <<"Error when opening "<<infile<<"..."<<endl;
	} else {
		outputfile << reg[rd] << endl << endl;
		outputfile.close();
	}
	clock += RWTICK; 
	timeSlice += RWTICK; 
    statusreg.sr.IO_REGISTER = rd;	// Register involved
    statusreg.sr.VM_RETURN_STATUS = 7;	// 001 write instruction	
}

void VirtualMachine::halt(){
	clock += TICK;
	timeSlice += TICK;
    statusreg.sr.VM_RETURN_STATUS = 1;	
}
void VirtualMachine::noop(){

}
/***********************ERRORS******************************
* error(10) - (LOAD()) - File failed to open.
* error(11) - (FETCH()) - Program out of bounds
* error(12) - (checkOverflow) - An overflow has occured:
* error(13) - (checkOverflow) - An overflow has occured:
* error(14) - (checklimit) - Program counter exceeds limit
***********************************************************/
