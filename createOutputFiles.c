#include "assembler.h"

/*This function creates the output file ".ob" which contains the final encoding table in base 32.
 *Parameter 1: fileName- name of the file.
 *Parameter 2: instructionList- instruction linked-list.
 *Parameter 3: dataList- data linked-list.
 *Parameter 4: IC- instruction counter.
 *Parameter 5: DC- data counter.
 */
void create_object_file(char *fileName, binaryNode **instructionList, binaryNode **dataList, int IC, int DC)
{
    FILE *fPointer;

    /*Declare a temporary variable for the file name, so that we do not change the original file name for the following outPut files*/
    char tempFileName[FILENAME_MAX];
    strcpy(tempFileName, fileName);
    change_ending_file_name(tempFileName);

    /*Opening a new file for writing.
     *The name of the file is the original name + ".ob" (the required suffix).*/
    fPointer = fopen(strcat(tempFileName, ".ob"), "w");

    /*Printing to file the number of instructions and the number of data words*/
    fprintf(fPointer, "%s\t%s\n", convert_to_Base32(IC-DC-100, TRUE, FALSE), convert_to_Base32(DC, TRUE, FALSE));
    /*Printing the words to the file in base 32*/
    print_to_file_object(fPointer, instructionList, dataList);

    fclose(fPointer);
    /*The necessary out-put file is ready (:*/
}

/*This function creates the output file ".ent" which contains all the symbols declared as entry and their address in base 32.
 *Parameter 1: fileName- name of the file.
 *Parameter 2: symbolTable- the symbol table (linked-list).
 */
void create_entries_file(char *fileName, symbol **symbolTable)
{
    FILE *fPointer;

    /*Declare a temporary variable for the file name, so that we do not change the original file name for the following outPut files*/
    char tempFileName[FILENAME_MAX];
    strcpy(tempFileName, fileName);
    change_ending_file_name(tempFileName);

    /*Checking that labels defined as entry do exist in the file.
     *Note: If there are no such labels, we will not create a new file of course, because there is no need*/
    if(existing_entry_label(symbolTable) == TRUE)
    {
        fPointer = fopen(strcat(tempFileName, ".ent"), "w");
        print_to_file_entries_labels(fPointer, symbolTable);
        fclose(fPointer);
    }
}

/*This function creates the output file ".ext" which contains all the symbols declared as extern and their address in base 32.
 *Parameter 1: fileName- name of the file.
 *Parameter 2: symbolTable- the symbol table (linked-list).
 *Parameter 3: instructionList- instruction linked-list.
 */
void create_externals_file(char *fileName, symbol **symbolTable, binaryNode **instructionList)
{
    FILE *fPointer;

    /*Declare a temporary variable for the file name, so that we do not change the original file name for the following outPut files*/
    char tempFileName[FILENAME_MAX];
    strcpy(tempFileName, fileName);
    change_ending_file_name(tempFileName);

    /*Checking that labels defined as extern do exist in the file.
     *Note: If there are no such labels, we will not create a new file of course, because there is no need*/
    if(existing_external_label(symbolTable) == TRUE)
    {
        fPointer = fopen(strcat(tempFileName, ".ext"), "w");
        print_to_file_externals_labels(fPointer, instructionList);
        fclose(fPointer);
    }
}

void change_ending_file_name(char *fileName)
{
    int index;

    for(index = 0; index < strlen(fileName); index++)
    {
        if(fileName[index] == '.')
        {
            fileName[index] = '\0';
            return;
        }
    }
}

char *convert_to_Base32(int numberToConvert, int firstLine, int data)
{
    char *base32Value = (char*) malloc(10 * sizeof(char));
    struct
    {
        char c;
        int value;
    } base[] = {{'!', 0},
                  {'@', 1},
                  {'#', 2},
                  {'$', 3},
                  {'%', 4},
                  {'^', 5},
                  {'&', 6},
                  {'*', 7},
                  {'<', 8},
                  {'>', 9},
                  {'a', 10},
                  {'b', 11},
                  {'c', 12},
                  {'d', 13},
                  {'e', 14},
                  {'f', 15},
                  {'g', 16},
                  {'h', 17},
                  {'i', 18},
                  {'j', 19},
                  {'k', 20},
                  {'l', 21},
                  {'m', 22},
                  {'n', 23},
                  {'o', 24},
                  {'p', 25},
                  {'q', 26},
                  {'r', 27},
                  {'s', 28},
                  {'t', 29},
                  {'u', 30},
                  {'v', 31},
                  };

    int reminder;
    int base32Divider = 32;
    int i;
    int j = 0;

    if(numberToConvert == 0)
    {
        base32Value[0] = '!';
        base32Value[1] = '!';
        base32Value[2] = '\0';

        return base32Value;
    }

    if (numberToConvert < 0)
    {
        if (data == FALSE)
            numberToConvert = two_complement((numberToConvert << 2));
        else
            numberToConvert = two_complement(numberToConvert);
    }

    if (numberToConvert < 32 && firstLine == FALSE)
    {
        base32Value[0] = '!';
        reminder = numberToConvert % base32Divider;

        i = 0;
        while(i < 32)
        {
            if (base[i].value == reminder)
            {
                base32Value[1] = base[i].c;
                break;
            }
            i++;
        }

        base32Value[2] = '\0';
        return base32Value;
    }

    while (numberToConvert > 0)
    {
        reminder = numberToConvert % base32Divider;
        numberToConvert = numberToConvert / base32Divider;

        i = 0;
        while(i < 32)
        {
            if (base[i].value == reminder)
            {
                base32Value[j] = base[i].c;
                j++;
                break;
            }
            i++;
        }
    }

    base32Value[j] = '\0';
    fixBase32Value(base32Value, (int)strlen(base32Value) - 1);
    return base32Value;
}

void fixBase32Value(char *base32Value, int length)
{
    int start = 0;
    int end = length;

    char temp;

    while (start < end)
    {
        temp = base32Value[start];
        base32Value[start] = base32Value[end];
        base32Value[end] = temp;

        start++;
        end--;
    }
}

void print_to_file_object(FILE *fPointer, binaryNode **instructionList, binaryNode **dataList)
{
    binaryNode *tempInstruction = *instructionList;
    binaryNode *tempData = *dataList;

    if (*instructionList == NULL)
        return;

    while(tempInstruction != NULL)
    {
        fprintf(fPointer, "%s\t%s\n", convert_to_Base32(tempInstruction->decimalAddress, FALSE, FALSE), convert_to_Base32(tempInstruction->binaryWord, FALSE, FALSE));
        tempInstruction = tempInstruction->next;
    }

    while(tempData != NULL)
    {
        fprintf(fPointer, "%s\t%s\n", convert_to_Base32(tempData->decimalAddress, FALSE, TRUE), convert_to_Base32(tempData->binaryWord, FALSE, TRUE));
        tempData = tempData->next;
    }
}

void print_to_file_entries_labels(FILE *fPointer, symbol **symbolTable)
{
    symbol *temp = *symbolTable;

    while (temp != NULL)
    {
        if(temp->ARE == ENTRY_ON)
            fprintf(fPointer, "%s\t%s\n", temp->label, convert_to_Base32(temp->decimalAddress, FALSE, FALSE));

        temp = temp->next;
    }
}

void print_to_file_externals_labels(FILE *fPointer, binaryNode **instructionList)
{
    binaryNode *temp = *instructionList;

    while (temp != NULL)
    {
        if(current_binary_word_is_extern_label(temp->binaryWord) == TRUE)
            fprintf(fPointer, "%s\t%s\n", temp->label, convert_to_Base32(temp->decimalAddress, FALSE, FALSE));

        temp = temp->next;
    }

}

int two_complement(int negativeNumberToFix)
{
    int absoluteValue = negativeNumberToFix * (-1);
    int currentBit = 0;
    int firstBitLitShow = FALSE;

    while (currentBit < 10)
    {
        /*Finding the first bit that is lit in our number (in absoluteValue).*/
        if (firstBitLitShow == FALSE && (absoluteValue &  (1 << currentBit)))
        {
            firstBitLitShow = TRUE;
            currentBit++;
            continue;
        }

        /*We have found the first bit that is lit, now we will reverse the bits*/
        /*case 1: bit is on, turn it off.*/
        if (firstBitLitShow == TRUE && (absoluteValue & (1 << currentBit)))
            absoluteValue = (absoluteValue & ~(1 << currentBit));

        /*case 2: bit is off, turn it on.*/
        else if(firstBitLitShow == TRUE)
            absoluteValue = (absoluteValue | 1 << currentBit);

        currentBit++;
    }
    return absoluteValue;
}

int current_binary_word_is_extern_label(int numberToCheck)
{
    int currentBit = 0;
    int firstBit = 0;

    if(numberToCheck & (0 << firstBit))
        return FALSE;

    currentBit++;
    while(currentBit < 10)
    {
        if(numberToCheck & (1 << currentBit))
            return FALSE;
        currentBit++;
    }

    return TRUE;
}
