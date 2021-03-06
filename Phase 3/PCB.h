/****************************************************************************
* PCB.h
* Ryan Rady
* Phase 2 - Operating System
* Date 05.13.13
* Description: Holds PCB information
*****************************************************************************/  
#ifndef PCB_H
#define PCB_H
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

class SR_PCB{
public:
	unsigned carry:1;
	unsigned greater:1;
	unsigned equal:1;
	unsigned less:1;
    unsigned overflow:1;
	unsigned VM_RETURN_STATUS:3;
	unsigned IO_REGISTER:2;
};

union PCB_SR{	
	int i;
	SR_PCB SR;
};

class PCB {
public:
	PCB();
	int PCBpc, PCBsp, PCBclock, PCBinterruptTime, PCBbase, PCBlimit;
	PCB_SR PCBstatusRegister;
	vector<int> PCBreg;
	vector<int> PCBmem;
	string PCB_Process;
	int PCBwaitTime,PCBturn_around_time,PCBio_time, PCBstackSize;
 	static const int PCB_REG_FILE_SIZE = 4;
	int pageFaults;
};

PCB::PCB(){
	PCBpc = 0;
	PCBbase = 0;
	PCBlimit = 0;
	PCBinterruptTime = 0;
	PCBsp = 256;
	PCBreg = vector<int>(PCB_REG_FILE_SIZE,0);
	PCBmem = vector<int> (6);
	PCBwaitTime = 0;
	PCBturn_around_time = 0;
	PCBio_time = 0;		
	PCBclock = 0;
	PCBstackSize = 0;
	pageFaults = 0;
};

#endif
