# Makefile

prog.s: os
	-./os prog.s

add5.s: os
	-./os add5.s

fact.s: os
	-./os fact.s

os: VirtualMachine.o Assembler.o
	g++ -o os os.cpp VirtualMachine.o Assembler.o

VirtualMachine.o: VirtualMachine.cpp
	g++ -c VirtualMachine.cpp

Assembler.o: Assembler.cpp
	g++ -c Assembler.cpp





