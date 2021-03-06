/****************************************************************************
* os.cpp
* Ryan Rady
* Phase 1 - Assembler Program 
* Date 04.29.13
* Description: runs assembler and virtual machine.
*****************************************************************************/  
#include "Assembler.h"
#include "VirtualMachine.h"

using namespace std;


int main(int argc, char *argv[]) 
{
	//outputs info into terminal, to display if the programs are working as intended.
	cout << " -- Reading Program Arguments -- " << endl;
	printf( "argc = %d\n", argc );
    	for( int i = 0; i < argc; ++i ) {
        	printf( "argv[ %d ] = %s\n", i, argv[ i ] );
	}

	//initialization
	Assembler as;
	VirtualMachine vm;

	//opens the file.
	fstream ofile;
	string file = argv[1];
	ofile.open(file.c_str());

 	if(ofile.fail()) {
   		cout <<"Error when opening "<< file << endl;
 	}

	//sends file to assembler and Virtual Machine.
	as.assemble(file);
	vm.LOAD(file);
	vm.FETCH();

	//system("PAUSE"); //necessary for vs2010.

	return 0;
}//main
