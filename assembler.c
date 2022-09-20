#include "assembler.h"

int main(int argc, char* argv[]) {

    FILE *fPointer;
    int currentFile = 1;

    int IC, DC;
    symbol *symbolTable;
    binaryNode *instructionList, *dataList;

    int i, errorFound;

    /*All the tests we will run in the file integrity testing procedure*/
    int (*test[NUMBER_OF_TESTS])(FILE* , char*);
    test[0] = check_max_chars_in_line;
    test[1] = check_opCode_exist;
    test[2] = check_number_of_operands;
    test[3] = check_operand_type;
    test[4] = check_for_double_opcode;
    test[5] = check_semicolon_appears_in_beginning_of_line;
    test[6] = check_labels_is_not_reserved_words_in_assembly;
    test[7] = check_declaration_of_constant_operand;
    test[8] = check_max_30_chars_in_label;
    test[9] = check_the_file_name_is_correct;
    test[10] = check_the_register_name_is_correct;
    test[11] = check_for_double_label;
    test[12] = check_label_is_not_both_entry_and_extern;
    test[13] = check_all_numbers_are_integers;

    /*Check if a file name has been received from the terminal*/
    if(argc == 1)
    {
        printf("No files were received, Good-Bye\n");
        exit(1);
    }

    /*Run over all files have been received, file-by-file*/
    while(currentFile < argc)
    {
	/*Phase 1: delete the blank character '\t' So that we can read word by word from the file freely and in an orderly manner*/
	fPointer = fopen(argv[currentFile], "r+");
	delete_space_tab(fPointer);
	fclose(fPointer);
	
        /*Phase 2: macro-retirement (pre-processor)*/
        /*Open the current file for reading*/
	fPointer = fopen(argv[currentFile], "r");
	rewind(fPointer);
        /*Start the pre-processor on the current file*/
        fPointer = macro_retirement(fPointer, argv[currentFile]);
        rewind(fPointer);
        /*Now the current file is after macro retirement*/


        /*Phase 3: validation process- here we check if the current file is syntactically correct according to the Assembly language
         *If the current file is syntactically incorrect, we stop the process on the current file and we will move on to next file.*/
        /*Start tests*/
        errorFound = FALSE;
        for(i = 0; i < NUMBER_OF_TESTS; i++)
        {
            if((*test[i])(fPointer, argv[currentFile]) == TRUE)
            {
                errorFound = TRUE;
            }
            rewind(fPointer);
        }
        if(errorFound == TRUE)
        {
            /*Move on to next file*/
            fclose(fPointer);
            currentFile++;
            continue;
        }
        /*End tests*/


        /*Phase 4: the current file syntactically correct according to the Assembly language.
         *now we will do the: first pass, second pass.*/
        first_pass(fPointer, &IC, &DC, &symbolTable, &instructionList, &dataList);
        rewind(fPointer);
        second_pass(&instructionList, &symbolTable);
        rewind(fPointer);

        /*Phase 5: create output files*/
        create_object_file(argv[currentFile] ,&instructionList, &dataList, IC, DC);
        create_entries_file(argv[currentFile], &symbolTable);
        create_externals_file(argv[currentFile], &symbolTable,&instructionList);

        /*Phase 6: free all memory allocated during the loop*/
        deallocate_binary_words(&instructionList);
        deallocate_binary_words(&dataList);
        deallocate_symbol_table(&symbolTable);

        /*Move on to next file*/
        fclose(fPointer);
        currentFile++;
    }

    /*We must say that we are happy to be here, the mission was accomplished, Thanks for checking our project.
     *Most of all! thank you for the opportunity to work on such an extensive project and learn so much.*/
    return 0;
}
