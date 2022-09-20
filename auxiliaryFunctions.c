#include "assembler.h"

/*This function delete all space-tab in current file.
 *Parameter 1: fPointer- the current file we want to delete the space-tab at.
 */
void delete_space_tab(FILE *fPointer)
{
    char currentChar;
    int idx;

    /*Go over all the characters in file*/
    FOREVER
    {
        /*Read the current char in file and save it in 'currentChar' var*/
        currentChar = (char)fgetc(fPointer);
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;
        if(currentChar == '\t')
        {
            idx = ftell(fPointer);
	    fseek(fPointer, idx - 1, SEEK_SET);
            fputc(' ', fPointer);
	}
    }
}

/*This function return the opCode value in decimal base.
 *If the received argument(fOpCode) is not an opcode then -1 will be returned.
 *Parameter 1: fOpCode- the word we want to check.
 */
int get_opCode_value(char *fOpCode)
{
    group opCodes[] = {{"mov", 0},
                            {"cmp", 1},
                            {"add", 2},
                            {"sub", 3},
                            {"lea", 6},
                            {"not", 4},
                            {"clr", 5},
                            {"inc", 7},
                            {"dec", 8},
                            {"jmp", 9},
                            {"bne", 10},
                            {"get", 11},
                            {"prn", 12},
                            {"jsr", 13},
                            {"rts", 14},
                            {"hlt", 15}};

    int runOverGroup = 0;

    while (runOverGroup < 16)
    {
        if(strcmp(opCodes[runOverGroup].word, fOpCode) == 0)
            return opCodes[runOverGroup].x;

        runOverGroup++;
    }

    return -1;
}

/*This function return the register value in decimal base.
 *If the received argument(currentWord) is not an register then -1 will be returned.
 *Parameter 1: currentWord- the word we want to check.
 */
int get_register_value(char *currentWord)
{
    group registersGroup[] = {{"r0", 0},
                                {"r1", 1},
                                {"r2", 2},
                                {"r3", 3},
                                {"r4", 4},
                                {"r5", 5},
                                {"r6", 6},
                                {"r7", 7}};

    int runOverGroup = 0;

    while (runOverGroup < 8)
    {
        if(strcmp(registersGroup[runOverGroup].word, currentWord) == 0)
            return registersGroup[runOverGroup].x;

        runOverGroup++;
    }

    return -1;
}

/*This function gets a number in decimal base and prints it in binary base.
 *Note: A useful function that helped a lot in writing the software, but now has no practical use in the project process.
 *Parameter 1: numberToConvert- the decimal number we want to print in binary.
 */
void print_binary(int numberToConvert)
{
    int bitsArray[10] = {0}; /*The size of binary-word is 10 bits, by the project definition*/
    int i = 0;

    while (numberToConvert > 0)
    {
        bitsArray[i] = numberToConvert % 2;
        numberToConvert = numberToConvert / 2;
        i++;
    }

    for(i = 9; i >= 0; i--)
    {
        printf("%d", bitsArray[i]);
    }
}

/*This function return the instruction value in decimal base.
 *If the received argument(currentWord) is not an instruction word then -1 will be returned.
 *Parameter 1: currentWord- the word we want to check.
 */
int get_instruction_value(char *currentWord)
{
    group instructionOptions[] = {{".entry", 1},
                                    {".extern", 2},
                                    {".data", 3},
                                    { ".string", 4},
                                    {".struct", 5}};

    int runOverGroup = 0;

    while (runOverGroup < 5)
    {
        if(strcmp(instructionOptions[runOverGroup].word, currentWord) == 0)
            return instructionOptions[runOverGroup].x;

        runOverGroup++;
    }

    return -1;
}

/*This function checks whether the word received as an argument has been declared as extern in the symbol table.
 *Parameter 1: head- the root of symbol-table (linked-list).
 *Parameter 2: currentWord- the word we want to check.
 */
int check_if_extern_label(symbol *head, char *currentWord)
{
    symbol *temp = head;

    if(head == NULL || currentWord == NULL)
    {
        return -1;
    }

    while(temp != NULL)
    {
        if (temp->ARE == EXTERN && strcmp(temp->label, currentWord) == 0)
            return 0;
        temp = temp->next;
    }
    return -1;
}

/*This function checks whether the word received as an argument has been declared as entry in the symbol table.
 *Parameter 1: currentWord- the word we want to check.
 *Parameter 2: head- the root of symbol-table (linked-list).
 */
int label_was_declared_as_an_entry(char *currentWord, symbol **symbolTable)
{
    symbol *temp = *symbolTable;

    while (temp != NULL)
    {
        if(strcmp(currentWord, temp->label) == 0 && temp->ARE == ENTRY)

            return TRUE;

        temp = temp->next;
    }

    return FALSE;
}

/*This function checks if there is a label in the symbol table declared as an entry.
 *Parameter 1: symbolTable- the symbol-table (linked-list).
 */
int existing_entry_label(symbol **symbolTable)
{
    symbol *temp = *symbolTable;

    while (temp != NULL)
    {
        if(temp->ARE == ENTRY)
            return TRUE;

        temp = temp->next;
    }

    return FALSE;
}

/*This function checks if there is a label in the symbol table declared as an extern.
 *Parameter 1: symbolTable- the symbol-table (linked-list).
 */
int existing_external_label(symbol **symbolTable)
{
    symbol *temp = *symbolTable;

    while (temp != NULL)
    {
        if(temp->ARE == EXTERN)
            return TRUE;

        temp = temp->next;
    }

    return FALSE;
}

/*This function checks whether the current word is actually an empty character.
 *Parameter 1: word- the current word we want to check.
 */
int check_blanked_space(char *word)
{
    if(strcmp(word, " ") == 0 || strcmp(word, "\t") == 0 || strcmp(word, "\n") == 0)
        return TRUE;
    return FALSE;
}



