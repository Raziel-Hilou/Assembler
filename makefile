assembler : assembler.c preProcessor.c validationTests.c auxiliaryFunctions.c linkedListFunctions.c firstPass.c secondPass.c createOutputFiles.c assembler.h
	gcc -o -ansi -pedantic -Wall assembler.c preProcessor.c validationTests.c auxiliaryFunctions.c linkedListFunctions.c firstPass.c secondPass.c createOutputFiles.c -o assembler -g -std=c90
