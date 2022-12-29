# Assembler
This is a large-scale project in the C language that I wrote during my university studies.
We were asked to write an assembler - software that receives source files in assembly language and converts it to binary and even more to the base 32 that was defined for us.                                                                        
Note: In short, the heart of the program(main function) is in file "assembler.c".

--This is a document explaining how to run the program and what the output is, We will not elaborate here on how the program is carried out.--

TO run this program on Linux system follow those steps-
1. Download the program to the Linux system.
2. Extract all the files from the zip.
3. Enter the Linux terminal, from the folder you extracted the files to, and run the following command - make.
4. Now in the terminal write the following command- ./assembler [the assembly source file you wish to convert]

OutPut Files-
After running the program on a source coder in assembly language, if the source file is correct in terms of assembly syntax, then up to 4 new files can be created.
1. Object file- Contains the machine code in base 32.
Where the first line contains the total number of instructions(left col) and the total number of data(right col) identified in the assembly source file you were run.
Now we will see the memory image so that the encoding of the first instruction from the assembly file will be entered for address 100 on our imaginary computer.
Left colum: The address of the binary word in memory.
Right colum: The content of the binary word.

2. Entries file- If labels were declared as entry in the assembly file, you will see them here.                                                                          Note- If no labels declared as entry, this output file will not be created.

3. Externals file- If labels were declared as extern in the assembly file, you will see them here.
Note- If no labels declared as extern, this output file will not be created.

4. am file- Contains the source file after preProccesor (deployment of macros).
Note- If no macros declared, this output file will not be created.
 
--base 32--
!,<,>,*,&,^,%,$,#,@,! a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v
(0 = !, < = 1, > = 2, * = 3, ..., v = 31)

Note- Example for step 4: ./assembler input1.as input2.as (as much file as you wish to convert)
Note- In the zip file you download, there is an assembly language source file for the experiment: 'input.as'
