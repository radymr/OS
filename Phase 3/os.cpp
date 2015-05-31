/****************************************************************************
* os.cpp
* Ryan Rady
* Phase 3 - Operating System
* Date 06.10.13
* Description: runs assembler and virtual machine.
*****************************************************************************/  
#include "Assembler.h"
#include "VirtualMachine.h"
#include "PCB.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <list>
#include <queue>
#include <cmath>

using namespace std;

class VirtualMachine;

//// Operating System //// 

class operatingSystem {
public:
	operatingSystem();			//constructor
	void osInitialize();		//gathers programs for progs.txt
	void osRun();				//does the pcb switching
	void osSave(PCB *inPCB);	//saves data in pcb
	void osLoad(PCB *inPCB);	//loads data from pcb
	void print(PCB *p);			//testing purposes.
	void transpose();			//outputs pcb and clock data to all '.out' files. 
	char flag;
private:
	int idleTime, interruptTime, waitTime, userTime, ioTime, switchTime;
	list<PCB *> jobs;
	queue<PCB *> readyQ,waitQ;
	Assembler as;
	VirtualMachine vm;
	string inFile;
	PCB * running;
};

//constructor
operatingSystem::operatingSystem() {
	idleTime = 0;
	interruptTime = 0;
	waitTime = 0;
	userTime = 0;
	ioTime = 0;
	switchTime = 0;
}

// Opens progs.txt, loads each .s file into it
// then creates a PCB for each program, and loads the data into it,
void operatingSystem::osInitialize() {
	//opens progs.txt to add the names of the programs to be ran.
	cout << " -- Initializing Operating System -- " << endl;
	ofstream outputFile;
	string file = "progs.txt";
	outputFile.open(file.c_str());
	
	//error check
	if(outputFile.fail())
   		cout << "Error when opening " << file << endl;
	
	//system call to gather all '.s' files and intput their names into progs.txt
	system("ls *.s > progs.txt");
	
	//closes progs.txt
	outputFile.close();
	
	//opens progs.txt for input
	ifstream inputFile;
	inputFile.open(file.c_str());

	// gets the '.s' file's
	getline(inputFile,inFile); 	

	//used to maintian the base of proceeding processes.
	int tempbase = 0;
	
	//creates new PCB, and adds each program in the jobs queue.
	while(!inputFile.eof()) {
 		PCB * p = new PCB;
		p -> PCB_Process = inFile;	
		jobs.push_back(p);		
		
		cout << "job: " << p->PCB_Process << endl;

		//output to log.txt
		fstream logOut;
		logOut.open("log.txt", fstream::in | fstream::out | ios::app);
		logOut << "job: " << p->PCB_Process << endl;
		logOut.close();
	
		readyQ.push(p);				
    	as.assemble(inFile);
		vm.base = tempbase;		
		vm.LOAD(inFile);	
		
		//loads pcb with data currently in virtual machine.
		p-> PCBbase =  vm.base;
		p-> PCBpc = p-> PCBbase;
		p-> PCBlimit = vm.limit;
		
		tempbase =+ vm.limit;
		osSave(p);
		getline(inputFile,inFile);
	}
	inputFile.close();
	osRun();
}

void operatingSystem::osRun(){

	ioTime = 0;
	while((waitQ.size() + readyQ.size()) != 0) {
			if(waitQ.size() != 0) {
				if(waitQ.front() -> PCBinterruptTime <= vm.clock) {	
		   			running = waitQ.front();
					waitQ.pop();
		   			readyQ.push(running);
				}
			}
		//retrieves first job in ready Q
		running = readyQ.front();

		//loads data to VM from PCB
		vm.infile = running -> PCB_Process;
		vm.base = running -> PCBbase;
		vm.limit = running -> PCBlimit;
		vm.pc = running -> PCBpc;
		vm.sp = running -> PCBsp;
		vm.reg = running -> PCBreg;
		vm.clock = running ->PCBclock;
		vm.statusreg.i = running -> PCBstatusRegister.i;
		vm.statusreg.sr.IO_REGISTER = running -> PCBstatusRegister.SR.IO_REGISTER;
		vm.mem[vm.sp+1] = running ->PCBmem[0];
		vm.mem[vm.sp+6] = running ->PCBmem[5];
	
		//send PCB to .st file.
		osLoad(running);
		
		//Runs the process through the Virtual Machine, FETCH-EXECUTE.
		vm.FETCH();     

		//loading everything 
		running -> PCB_Process = vm.infile;
		running -> PCBlimit = vm.limit;
		running -> PCBbase = vm.base;
		running -> PCBpc = vm.pc;
		running -> PCBstatusRegister.i = vm.statusreg.i;
		running -> PCBclock = vm.clock;
		running -> PCBsp = vm.sp;
		running -> PCBreg = vm.reg; 
		running -> PCBstatusRegister.SR.IO_REGISTER = vm.statusreg.sr.IO_REGISTER;
		running -> PCBwaitTime = idleTime;
		running -> PCBturn_around_time = vm.clock;
		running -> PCBio_time = ioTime; 
		running -> PCBmem[0] = vm.mem[vm.sp+1];
		running -> PCBmem[5] = vm.mem[vm.sp+6];
		
		//save PCB state.
		osSave(running);

		// for output
		string tempData; 
		string ofile = running->PCB_Process.substr(0,running->PCB_Process.size()-1);
		ofile = ofile + "out"; 
		fstream outputfile;

		//return status's
		switch(vm.statusreg.sr.VM_RETURN_STATUS){
			case 0:
				if (vm.statusreg.sr.PAGE_FAULT_STATUS == 1){
						cout << " Page Fault(os)" << endl;
					running->pageFaults++;
						cout << "pageFaults " << running->pageFaults << endl;
					vm.clock += vm.CSTICK;
					running = readyQ.front();							
					readyQ.push(running);
					readyQ.pop();
					//vm.pc = running -> PCBpc;
					vm.statusreg.sr.PAGE_FAULT_STATUS = 0;
					vm.LOAD(running -> PCB_Process);
					vm.pageFaults = running->pageFaults;
				} else {
				if (flag == '2')
					cout << "Time slice" <<endl;
				vm.clock += vm.CSTICK;
				running = readyQ.front();							
				readyQ.push(running);
				readyQ.pop();
				}
				break;
			case 1: 
				if (flag == '2')
					cout <<"Halt Instruction" <<endl;
				vm.clock += vm.CSTICK;
				running ->PCBclock = vm.clock;
				userTime += vm.clock;
				osSave(running);
				running->PCBstatusRegister.SR.VM_RETURN_STATUS = 0;

				//output Process Data.
				outputfile.open(ofile.c_str());
			
				if(outputfile.fail()){
					cout <<"Error when opening "<<ofile<<"..."<<endl;
				} else {
					outputfile >> tempData;
					outputfile << endl << endl << "Process Data: " << endl;
					outputfile << "CPU Time: " << running->PCBclock << endl;
					outputfile << "Waiting Time: " << running->PCBwaitTime << endl;
					outputfile << "I/O Time: " << running->PCBio_time << endl;
					running->PCBstackSize = running->PCBlimit - running->PCBbase;
					outputfile << "Memory Stack Size: " << running->PCBstackSize << endl;
		
					outputfile.close();
				}
				
				readyQ.pop();
				if(readyQ.empty() and !waitQ.empty()) {	
					PCB *temp = waitQ.front();
					int time_difference = abs(temp-> PCBinterruptTime - vm.clock);
					vm.clock += time_difference;
					idleTime += time_difference;
				}
				break;
			case 2: 
				cout <<"Out-of-bound Reference"<<endl;
				vm.clock += vm.CSTICK;
				break;
			case 3: 
				cout <<"Stack Overflow"<<endl;
				vm.clock += vm.CSTICK;
				break;
			case 4: 
				cout <<"Stack Underflow"<<endl;
				vm.clock += vm.CSTICK;
				break;
			case 5: 
				cout <<"Invalid Opcode"<<endl;
				vm.clock += vm.CSTICK;
				break;
			case 6: 
				if (flag == '2')
					cout <<"Read Operation"<<endl;
				ioTime += 27;
				vm.clock += vm.CSTICK;
				osSave(running);
				running->PCBstatusRegister.SR.VM_RETURN_STATUS = 0;
				interruptTime = vm.clock + 28;
				running->PCBinterruptTime = interruptTime;
				waitQ.push(running);
				readyQ.pop();

				if(readyQ.empty() and !waitQ.empty()){	
					PCB *temp = waitQ.front();
					int time_difference = abs(temp-> PCBinterruptTime - vm.clock);
					vm.clock += time_difference;
					idleTime += time_difference;
				}

				break;
			case 7: 
				if (flag == '2')
					cout <<"Write Operation"<<endl;
				ioTime += 27;
				vm.clock += vm.CSTICK;
				osSave(running);
				running->PCBstatusRegister.SR.VM_RETURN_STATUS = 0;
				interruptTime = vm.clock + 28;
				running->PCBinterruptTime = interruptTime;
				waitQ.push(running);
				readyQ.pop();

				if(readyQ.empty() and !waitQ.empty()){	
					PCB *temp = waitQ.front();
					int time_difference = abs(temp-> PCBinterruptTime - vm.clock);
					vm.clock += time_difference;
					idleTime += time_difference;
				}

				break;
			}

		switchTime += 10;
	}//while
	//when program counter is equal to the limit of the process, that process is complete. 
	if(running->PCBpc == running->PCBlimit )
		//eliminates the process, its no longer needed.									
		readyQ.pop();	
}

void operatingSystem::osSave(PCB *inPCB){
	string filename = inPCB->PCB_Process;
	string stfile = filename.substr(0,filename.size()-2);
    stfile = stfile + ".st"; 
	ofstream outfile;
	outfile.open(stfile.c_str(),ios::out);
	if(outfile.fail()){
	   cout <<"Error when opening "<< stfile <<endl;
 	}

	outfile << "inPCB->PCBpc    " << inPCB->PCBpc << endl;
	outfile << "inPCB->PCBbase    " << inPCB->PCBbase << endl;
	outfile << "inPCB->PCBlimit    " << inPCB->PCBlimit << endl;
	outfile << "inPCB->PCBinterruptTime    " << inPCB->PCBinterruptTime << endl;
	outfile << "inPCB->PCBsp    " << inPCB->PCBsp << endl;
	outfile << "inPCB->PCBwaitTime    " << inPCB->PCBwaitTime << endl;
	outfile << "inPCB->PCBio_time    " << inPCB->PCBio_time << endl;
	outfile << "inPCB->PCBclock    " << inPCB->PCBclock << endl;
	for (int i = 0; i < 4; i ++)
		outfile << "reg[" << i << "]: " << inPCB->PCBreg[i] << endl;
	for (int j = 0; j< 6; j++)
		outfile << "mem: " << inPCB->PCBmem[j] << endl;
	outfile.close();

	//output to log.txt
	fstream logOut;
	logOut.open("log.txt", fstream::in | fstream::out | ios::app);
	logOut << endl << "Process: " << inPCB->PCB_Process;
	logOut << endl << "inPCB->PCBpc    " << inPCB->PCBpc << endl;
	logOut << "inPCB->PCBbase    " << inPCB->PCBbase << endl;
	logOut << "inPCB->PCBlimit    " << inPCB->PCBlimit << endl;
	logOut << "inPCB->PCBinterruptTime    " << inPCB->PCBinterruptTime << endl;
	logOut << "inPCB->PCBsp    " << inPCB->PCBsp << endl;
	logOut << "inPCB->PCBwaitTime    " << inPCB->PCBwaitTime << endl;
	logOut << "inPCB->PCBio_time    " << inPCB->PCBio_time << endl;
	logOut << "inPCB->PCBclock    " << inPCB->PCBclock << endl;
	for (int i = 0; i < 4; i ++)
		logOut << "reg[" << i << "]: " << inPCB->PCBreg[i] << endl;
	for (int j = 0; j< 6; j++)
		logOut << "mem: " << inPCB->PCBmem[j] << endl;
	logOut << endl;
	logOut.close();
}

//Load data from PCB.
void operatingSystem::osLoad(PCB *inPCB){
	string filename = inPCB->PCB_Process;
	string stline, temp;	
	int data;				
	int i = 256;
	string stfile = filename.substr(0,filename.size()-2);
	stfile = stfile + ".st"; 
  	ifstream infile;
	infile.open(stfile.c_str(), ios::in);

	if (!infile.is_open()) {
		cout << stfile <<" failed to open.\n";
		//exit(1);
	}

	infile >> temp >> inPCB->PCBpc;
	infile >> temp >> inPCB->PCBbase;
	infile >> temp >> inPCB->PCBlimit;
	infile >> temp >> inPCB->PCBinterruptTime;
	infile >> temp >> inPCB->PCBsp;
	infile >> temp >> inPCB->PCBwaitTime;
	infile >> temp >> inPCB->PCBio_time;
	infile >> temp >> inPCB->PCBclock;

	for (int i = 0; i < 4; i ++)
		infile >> temp >> temp >> temp >> inPCB->PCBreg[i];
	for (int j = 0; j< 6; j++)
		infile >> temp >> inPCB->PCBmem[j]; 

	infile.close();
}

//for testing purposes.
void operatingSystem::print(PCB * p) {

	
	cout << "Testing purposes" << endl;
	cout << "p-> PCB_Process: " << p-> PCB_Process << endl;
	cout << "p-> PCBpc: " << p-> PCBpc << endl;
	cout << "p-> PCBbase: " << p-> PCBbase << endl;
	cout << "p-> PCBlimit: " << p-> PCBlimit << endl;
}

void operatingSystem::transpose() {
	//system data.
	cout << " -- Transposing Data -- " << endl;
	// sum of all Context Switch Tumes and Idle Times.
	double finalClock;			
	// (final clock - sum of all idle times) / final clock (%)
	double cpuUtilization;		
	// (sum of all jobs' CPU time ) / final clock (%)
	double userUtilization;	
	// number of processes complete per second. 1second = 10000 ticks.  
	double throughput;			

	//Formulating the CPU data.
	finalClock = idleTime + switchTime;
	cpuUtilization = (finalClock - idleTime) / finalClock;
	userUtilization = userTime / finalClock;
	throughput = finalClock / 10000 ;

	//opens progs.txt to use in next step
	ifstream inFile;
	string file = "progs.txt";
	inFile.open(file.c_str());
	if (!inFile.is_open()) 
		cout << inFile <<" failed to open.\n";
	string inProcess;
	int tempData;

	//opens every file listed in progs.txt to add CPU data to the end of the .out file.
	while(!inFile.eof()){
		getline(inFile, inProcess); 

		string ofile = inProcess.substr(0,inProcess.size()-2);
    	ofile = ofile + ".out";

		ofstream outFile; 
		outFile.open(ofile.c_str(), ios::app);
		if (!outFile.is_open()) 
			cout << outFile <<" failed to open.\n";

		outFile.setf(ios::fixed);
		outFile.setf(ios::showpoint);
		outFile.precision(2); 
		outFile << endl <<  "CPU Data: " << endl;
		outFile << "Final Clock : " << finalClock << endl;
		outFile << "CPU Utilization : " <<  cpuUtilization*100 << "%" << endl;
		outFile << "User Utilization : " <<  userUtilization*100 << "%" << endl;
		outFile << "Throughput : " << throughput << endl;
		outFile.close();
	}
	inFile.close();
}

/////////////////////////////////////////////////////////////////////
/////////////////////////// MAIN FUNCTION ///////////////////////////
/////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {

	//there will also a "log" output file that will have all of the test output information.
	//this clears that log file before being opened inside the OS.
	ofstream logClear("log.txt");
	logClear.close();

	//output to log.txt
	fstream logOut;
	logOut.open("log.txt", fstream::in | fstream::out | ios::app);
	logOut << " -- Initializing Operating System -- " << endl;
	logOut.close();

	//initializes an operating system.
	operatingSystem os;

	//used to output system information for testing purposes.
	//the second flag being input from linux terminal line should be a 1 or a 2.
	os.flag = *argv[2];

	//checks if the flag is valid.
	if (os.flag == '1' || os.flag == '2')
		cout << "Test mode activated with flag: " << os.flag << endl;
	
	//outputs what the flag will allow as an output.
	if (os.flag == '1')
		cout << "   output will include: something something." << endl;
	else if (os.flag == '2')
		cout << "   output will include: something something." << endl;

	//within the OS the assembler and virtual machine will be created.
	os.osInitialize();

	//outputs pcb and clock data to all '.out' files. 
	os.transpose();

	//this is output as long as everything has worked properly.	
	cout << "SUCCESS" <<endl;

	return 0;

}//main
