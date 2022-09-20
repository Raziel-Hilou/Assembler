#include "assembler.h"

/*This source file contains all the input integrity checks we will perform on the file we receive from the user.
 *If one of the functions returns "TRUE" then an error has been detected and the output files will not be created.
 *Note: All these functions (test functions) receive exactly 2 parameters.
 *Parameter 1: fPointer- the file we are testing.
 *Parameter 2: fileName- the name of the file we testing
 */

/*This function check that there is no line containing more than 80 characters*/
int check_max_chars_in_line(FILE *fPointer, char *fileName)
{
    int currentLine = 1;
    int currentChar;
    int numberOfCharsInLine = 1;

    int errorFound = FALSE;

    /*Go over all the characters in file*/
    FOREVER
    {
        /*Read the current char in file and save it in 'currentChar' var*/
        currentChar = fgetc(fPointer);
        /*Check if we get to the end of the file*/
        if(feof(fPointer))
            break;

        /*if we get to the end of line, we will reset 'numberOfCharsInLine', so we can start to count the next line*/
        else if(currentChar == '\n')
        {
            numberOfCharsInLine = 1;
            currentLine++;
        }

        /*Blank characters are not count*/
        else if(currentChar == ' ' || currentChar == '\t')
            continue;

        /*if the file exceeding the allowed number of characters in a single line, error message will be print to screen*/
        else if(numberOfCharsInLine > MAX_CHARS_IN_LINE)
        {
            /*We found error*/
            errorFound = TRUE;

            printf("\nfile: %s | line: %d | Error 01: exceeding the allowed number of characters in a single line.\n", fileName,currentLine);
            printf("One line can contain a maximum of 80 characters.\n");

            /*For us this line has been tested, there is no point in continuing to check it.*/
            currentLine++;
            numberOfCharsInLine = 1;
            /*Move on to next line*/
            while(fgetc(fPointer) != '\n')
            {
                fseek(fPointer, ftell(fPointer), SEEK_SET);
            }
        }

        numberOfCharsInLine++;
        fseek(fPointer, ftell(fPointer), SEEK_SET);
    }

    return errorFound;
}

/*Checking that the opcodes in the file are part of the 16 opcodes allowed in the assembly language and that they are written correctly*/
int check_opCode_exist(FILE *fPointer, char *fileName)
{
    char currentLine[256];
    char *currentWord;

    int currentLineIndex = 1;
    int errorFound = FALSE;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if(feof(fPointer))
            break;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while(currentWord != NULL)
        {
            /*If the current word is blanked space move on to next word*/
            if(check_blanked_space(currentWord) == TRUE)
            {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if(currentWord[0] == ';')
                break;

            /*If the current word is symbol, move on to next word*/
            if(currentWord[strlen(currentWord) - 1] == ':')
            {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            /*If the current word is instruction word: ".string"/".data"/".struct"/".entry"/".extern"
             *move on to next line*/
            if (get_instruction_value(currentWord) != -1)
                break;

            /*If the current word is indeed opCode then our situation is normal, we will move to the next line.*/
            if(get_opCode_value(currentWord) != -1)
                break;

            /*If we got here then the current word should be opCode but it doesn't exist in opCodes list*/
            /*So we found error*/
            errorFound = TRUE;
            printf("\nfile: %s | line: %d | Error 02: This opCode -'%s'- does not exist.\n", fileName, currentLineIndex, currentWord);
            break;
        }
        currentLineIndex++;
    }
    return errorFound;
}

/*Checking that the correct number of operands is received for each opCode.*/
int check_number_of_operands(FILE *fPointer, char *fileName)
{
    char currentLine[256];
    char *currentWord;

    int currentLineIndex = 1;
    int operandsCounter;
    int opCode;
    int errorFound = FALSE;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if(feof(fPointer))
            break;

        /*In each new line we will reset the number of operands*/
        operandsCounter = 0;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while(currentWord != NULL)
        {
            /*If the current word is blanked space or symbol move on to next word*/
            if(strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0 || currentWord[strlen(currentWord) - 1] == ':')
            {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if(currentWord[0] == ';')
                break;

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            /*If the current word is instruction word: ".string"/".data"/".struct"/".entry"/".extern"
             *move on to next line*/
            if (get_instruction_value(currentWord) != -1)
                break;

            /*If the current word is indeed opCode then we will check that the number of operands is correct*/
            opCode = get_opCode_value(currentWord);
            if(opCode != -1)
            {
                /*case 1: opcode need to get two operands*/
                if((opCode >= 0 && opCode <= 3) || opCode == 6)
                {
                    /*Move on to first argument*/
                    currentWord = strtok(NULL, ", ");
                    /*count number of operands in current line*/
                    while (currentWord != NULL)
                    {
                        if(check_blanked_space(currentWord) == TRUE)
                        {
                            currentWord = strtok(NULL, " ");
                            continue;
                        }
                        currentWord = strtok(NULL, ", ");
                        operandsCounter++;
                    }

                    if(operandsCounter < 2)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | line: %d | Error 03: Missing arguments, the opCode in this line need to get two arguments.\n", fileName, currentLineIndex);
                        break;
                    }
                    if(operandsCounter > 2)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | line: %d | Error 03: Too many arguments, the opCode in this line need to get two arguments.\n", fileName, currentLineIndex);
                        break;
                    }
                }
                /*case 2: opcode need to get one operand*/
                if((opCode >= 7 && opCode <= 13) || opCode == 4 || opCode == 5)
                {
                    /*Move on to first argument*/
                    currentWord = strtok(NULL, ",");
                    /*count number of operands in current line*/
                    while (currentWord != NULL)
                    {
                        if(check_blanked_space(currentWord) == TRUE)
                        {
                            currentWord = strtok(NULL, " ");
                            continue;
                        }
                        currentWord = strtok(NULL, ",");
                        operandsCounter++;
                    }

                    if(operandsCounter < 1)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | line: %d | Error 03: Missing arguments, the opCode in this line need to get one argument.\n", fileName, currentLineIndex);
                        break;
                    }
		    if(operandsCounter > 1)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | line: %d | Error 03: Too many arguments, the opCode in this line need to get one argument.\n", fileName, currentLineIndex);
                        break;
                    }

                }
                /*case 3: opcode don't get operands at all*/
                if(opCode == 14 || opCode == 15)
                {
                    /*Move on to first argument*/
                    currentWord = strtok(NULL, ",");
                    /*count number of operands in current line*/
                    while (currentWord != NULL)
                    {
                        if(check_blanked_space(currentWord) == TRUE)
                        {
                            currentWord = strtok(NULL, " ");
                            continue;
                        }
                        currentWord = strtok(NULL, ",");
                        operandsCounter++;
                    }
                    if(operandsCounter != 0)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | line: %d | Error 03: Too many arguments, the opCode in this line don't get arguments at all.\n", fileName, currentLineIndex);
                        break;
                    }
                }
            }

            break;
        }
        currentLineIndex++;
    }

    return errorFound;
}

/*Checking that for each opCode the received operand types are correct*/
int check_operand_type(FILE *fPointer, char *fileName)
{
    char currentLine[256];
    char *currentWord;

    int currentLineIndex = 1;
    int opCode;
    int errorFound = FALSE;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while (currentWord != NULL)
        {
            /*If the current word is blanked space or symbol move on to next word*/
            if(strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0 || currentWord[strlen(currentWord) - 1] == ':')
            {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if(currentWord[0] == ';')
                break;

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            /*If the current word is instruction word: ".string"/".data"/".struct"/".entry"/".extern"
             *move on to next line*/
            if (get_instruction_value(currentWord) != -1)
                break;

            /*If the current word is indeed opCode then we will check that the follow of operands is correct*/
            opCode = get_opCode_value(currentWord);
            if(opCode != -1)
            {
                switch (opCode)
                {
                    /*mov*/
                    case 0:
                        /*move on to second operand*/
                        currentWord = strtok(NULL, ",");
                        currentWord = strtok(NULL, "\n");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;
                    /*add*/
                    case 2:
                        /*move on to second operand*/
                        currentWord = strtok(NULL, ",");
                        currentWord = strtok(NULL, "\n");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*sub*/
                    case 3:
                        /*move on to second operand*/
                        currentWord = strtok(NULL, ",");
                        currentWord = strtok(NULL, "\n");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*not*/
                    case 4:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*clr*/
                    case 5:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*lea*/
                    case 6:
                        currentWord = strtok(NULL, ",");
                        if(currentWord != NULL)
                        {
                            if (get_register_value(currentWord) != -1 || strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }

                            currentWord = strtok(NULL, "\n");
                            if(currentWord != NULL)
                            {
                                if (strstr(currentWord, "#") != 0)
                                {
                                    errorFound = TRUE;
                                    printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                           fileName, currentLineIndex);
                                }
                            }
                        }
                        break;

                    /*inc*/
                    case 7:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*dec*/
                    case 8:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*jmp*/
                    case 9:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*bne*/
                    case 10:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*get*/
                    case 11:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    /*jsr*/
                    case 13:
                        currentWord = strtok(NULL, " ");
                        if(currentWord != NULL)
                        {
                            if (strstr(currentWord, "#") != 0)
                            {
                                errorFound = TRUE;
                                printf("\nfile: %s | line: %d | Error 04: This opCode does not support that operand type.\n",
                                       fileName, currentLineIndex);
                            }
                        }
                        break;

                    default:
                        break;
                }

            }
            currentWord = strtok(NULL, " ");
        }
        currentLineIndex++;
    }
    return errorFound;
}

/*check that there is no line that contain two opCodes. This is illegal in assembly language.*/
int check_for_double_opcode(FILE *fPointer, char *fileName)
{
    char currentLine[256];
    char *currentWord;
    int currentLineIndex = 1;
    int counter = 0;
    int errorFound = FALSE;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while (currentWord != NULL)
        {
            /*If the current word is blanked space move on to next word*/
            if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0)
            {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if (currentWord[0] == ';')
            {
                break;
            }


            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            /*Check if current word is an opcode, and add 1 to counter accordingly. if counter = 2 print error */
            if(get_opCode_value(currentWord) != -1)
            {
                counter++;
                if (counter > 1)
                {
                    errorFound =TRUE;
                    printf("\nfile: %s | line: %d | Error 05: double opCode in same line, one line can contain only one opCode.\n", fileName, currentLineIndex);
                }
            }
            currentWord= strtok(NULL, " ");
        }

        /*Set counter to zero at the start of any line */
        counter = 0;
        currentLineIndex++ ;
    }

    return errorFound;
}

/*check that comment sign ';' is found only at the beginning of a line*/
int check_semicolon_appears_in_beginning_of_line(FILE *fPointer, char *fileName)
{
    char currentLine[256];
    char *currentWord;

    int currentLineIndex = 1;
    int counter=0;
    int errorFound = FALSE;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if(feof(fPointer))
            break;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while(currentWord != NULL )
        {

            /*If the current word is blanked space move on to next word*/
            if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0) {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            /*check if ';' appears in line from second word and on */
            if (strchr(currentWord, ';') != NULL && counter != 0) {
                errorFound = TRUE;
                printf("\nfile: %s | line: %d | Error 06: semicolon appears illegally, semicolon can appear only in beginning of line.\n", fileName, currentLineIndex);
                break;

            }

            /*checks if ';' appears illegally in first word. (counter == 0)  */
            if (strchr(currentWord, ';') != NULL && counter == 0) {
                if(currentWord[0]!=';'){
                    errorFound = TRUE;
                    printf("\nfile: %s | line: %d | Error 06: semicolon appears illegally, semicolon can appear only in beginning of line.\n", fileName, currentLineIndex);
                    break;
                }

            }
            currentWord= strtok(NULL, " ");
            counter++;
        }
        currentLineIndex++;
        counter=0;
    }

    return errorFound;
}

/*Check labels is not a reserved words in assembly language*/
int check_labels_is_not_reserved_words_in_assembly(FILE *fPointer, char *fileName)
{
    char currentLine[256];
    char *currentWord;

    int currentLineIndex = 1;
    int k;
    int errorFound = FALSE;

    fseek(fPointer, 0, SEEK_SET);

    FOREVER {
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while (currentWord != NULL) {

            /*If the current word is blanked space move on to next word*/
            if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0) {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';


            /*If the current word is symbol, compare to opcode instructions. return TRUE if match exist*/
            if (currentWord[strlen(currentWord) - 1] == ':') {
                currentWord[strlen(currentWord) - 1] = '\0';
                if (get_opCode_value(currentWord) != -1) {
		    printf("\nfile: %s | line: %d | Error 07: The declared label, in this line, is name of a reserved word in assembly language.\n", fileName, currentLineIndex);
                    errorFound = TRUE;
                }

                /* check if current label is a register name */
                if (currentWord[0] == 'r') {
                    while (1) {

                        if ((isdigit(currentWord[1]) == 0))
                            break;

                        for (k = 0; k < 8; k++) {
                            if (get_register_value(currentWord) == k) {
                                printf("\nfile: %s | line: %d | Error 07: The declared label, in this line, is name of a reserved word in assembly language.\n", fileName, currentLineIndex);
                                errorFound = TRUE;
                                break;
                            }
                        }

                        break;
                    }
                }

                /* check if label defined as entry or extern or data or string or struct*/
                if(get_instruction_value(currentWord) != -1){
                    printf("\nfile: %s | line: %d | Error 07: The declared label, in this line, is name of a reserved word in assembly language.\n", fileName, currentLineIndex);
                    errorFound = TRUE;
                }


            }
            currentWord = strtok(NULL, " ");
        }
        currentLineIndex++;
    }
    return errorFound;
}

/*check that the '#' char is used legally*/
int check_declaration_of_constant_operand(FILE *fPointer, char *fileName)
{
    char currentLine[256];
    char *currentWord;
    int currentLineIndex = 1;
    int errorFound = FALSE;
    int i,j,flag=0;


    FOREVER {
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while (currentWord != NULL) {
            /*If the current word is blanked space move on to next word*/
            if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0) {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if (currentWord[0] == ';'){
                break;
            }


            /*If the current word is symbol, move on to next word*/
            if (currentWord[strlen(currentWord) - 1] == ':') {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            if(strstr(currentWord,",") !=0 ) {
                for (i = 0; i < strlen(currentWord); i++) {
                    if (currentWord[i] == ',')
                        currentWord[i] = ' ';
                }
            }

            if(strstr(currentWord,"#")) {
                for (i = 0; i < strlen(currentWord); i++) {
                    if (currentWord[i] == '#') {
                        for (j = i + 1; j < strlen(currentWord); j++) {

                            if(currentWord[j] == ' ')
                                break;

                            if (isdigit(currentWord[j]) == 0) {
                                flag = 1;
                                break;
                            }
                        }

                        if (flag && (!(currentWord[j] == '-' || currentWord[j] == ' ' || currentWord[j] == ',' || currentWord[j] == '\n' || currentWord[j] == '\t' || currentWord[j] == '.'))) {
                            printf("\nfile: %s | line: %d | Error 08: Illegal declaration of constant operand.\n",fileName, currentLineIndex);
                            errorFound = TRUE;
                            flag = 0;
                            break;
                        }
                    }
                }

                if (currentWord[0] == '#' && strlen(currentWord) == 1 ){
                    printf("\nfile: %s | line: %d | Error 08: Illegal declaration of constant operand.\n",fileName, currentLineIndex);
                    errorFound = TRUE;
                }
            }
	    flag = 0;
            currentWord= strtok(NULL, " ");
        }
        currentLineIndex++ ;

    }
    return errorFound;
}


/*check that every label does not contain more than 30 characters*/
int check_max_30_chars_in_label(FILE* fPointer,char *fileName)
{
    char currentLine[256];
    char *currentWord;
    int currentLineIndex = 1;
    int errorFound = FALSE;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while (currentWord != NULL)
        {
            /*If the current word is blanked space move on to next word*/
            if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0)
            {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if (currentWord[0] == ';')
            {
                break;
            }

            /*If the current word is symbol, move on to next word*/
            if (currentWord[strlen(currentWord) - 1] == ':')
            {
                if(strlen(currentWord)>30)
                {
                    errorFound=TRUE;
                    printf("\nfile: %s | line: %d | Error 09: The label declared on this line contains more than 30 characters, not possible.\n", fileName, currentLineIndex);
                    break;
                }
            }
            currentWord= strtok(NULL, " ");
        }
        currentLineIndex++ ;
    }
    return errorFound;
}

/*check that the file received as input is indeed a source file in assembly language*/
int check_the_file_name_is_correct(FILE* fPointer,char *fileName)
{
    int errorFound = FALSE;

    /*Declare a temporary variable for the file name, so that we do not change the original file name*/
    char *temp = fileName;
    fileName = fileName + (strlen(fileName) - 3);

    if(fileName == NULL)
    {
        printf("\nfile: %s | Error 10: This file is not an Assembly source-code, make sure the file name ends with '.as'.\n", temp);
        errorFound = TRUE;
        return errorFound;
    }

    if(strcmp(fileName, ".as") != 0)
    {
        printf("\nfile: %s | Error 10: This file is not an Assembly source-code, make sure the file name ends with '.as'.\n", temp);
        errorFound = TRUE;
    }

    return errorFound;
}

/*check that the name of the registers is correct*/
int check_the_register_name_is_correct(FILE* fPointer,char *fileName)
{
    char currentLine[256];
    char *currentWord;

    int currentLineIndex = 1;
    int errorFound = FALSE;

    FOREVER {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while (currentWord != NULL) {
            /*If the current word is blanked space or symbol move on to next word*/
            if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0 ||
                currentWord[strlen(currentWord) - 1] == ':') {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if (currentWord[0] == ';')
                break;

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            /*If the current word is instruction word: ".string"/".data"/".struct"/".entry"/".extern"
             *move on to next line*/
            if (get_instruction_value(currentWord) != -1)
                break;

            /*If the current word is indeed opCode then we will check that the follow of operands is correct*/
            if(get_opCode_value(currentWord) != -1)
            {
                /*check both operands*/
                currentWord = strtok(NULL, " ");
                while(currentWord != NULL)
                {
                    if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0)
                    {
                        currentWord = strtok(NULL, " ");
                        continue;
                    }
                    /*case 1: blanked space after register declare*/
                    if (strcmp(currentWord, "r") == 0)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | line: %d | Error 11: register number is illegal.\n", fileName, currentLineIndex);
			break;
                    }
                    /*case 2: number of register is illegal*/
                    if(currentWord[0] == 'r')
                    {
                        if (currentWord[1] < 48 || currentWord[1] > 55)
                        {
                            errorFound = TRUE;
			    printf("\nfile: %s | line: %d | Error 11: register number is illegal.\n", fileName, currentLineIndex);
			    break;
                        }
                    }
                    currentWord = strtok(NULL, ",");
                }
            }
            currentWord = strtok(NULL, " ");
        }
        currentLineIndex++;
    }
    return errorFound;
}

/*check that there is no label declared twice or more*/
int check_for_double_label(FILE *fPointer,char *fileName)
{
    char currentLine[256];
    char *currentWord = NULL;
    char currentLabelToCheck[30] = {0};
    int currentLineIndex = 1;
    int errorFound = FALSE;

    int lastLocation = 0;
    int tempLineIndex = 0;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");
        /*case: we found label, now we will compare it with all labels after her in file*/
        if(currentWord[strlen(currentWord)-1] == ':' && strlen(currentWord) <= 30)
        {
            /*fix word - ignore ':' char*/
            currentWord[strlen(currentWord)-1] = '\0';
            /*Hold the current label*/
            strcpy(currentLabelToCheck, currentWord);
            /*remember the last location we stop at*/
            lastLocation = ftell(fPointer);

            tempLineIndex = currentLineIndex;
            /*comparing process*/
            FOREVER
            {
                /*Hold the current line from file*/
                fgets(currentLine, 256, fPointer);
                currentLineIndex++;
                /*Check if we get to the end of the file*/
                if(feof(fPointer))
                {
                    fseek(fPointer, lastLocation, SEEK_SET);
                    break;
                }
                /*Hold the first word in line*/
                currentWord = strtok(currentLine, " ");
                /*check if we reached to another label*/
                if(currentWord[strlen(currentWord)-1] == ':')
                {
                    /*fix word - ignore ':' char*/
                    currentWord[strlen(currentWord)-1] = '\0';
                    if(strcmp(currentLabelToCheck, currentWord) == 0)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | Error 12: The label '%s' was declared twice.\nfirst appears in line: %d\nsecond appears in line: %d\n", fileName, currentWord,tempLineIndex, currentLineIndex);
                        fseek(fPointer, lastLocation, SEEK_SET);
                        break;
                    }
                }
            }
            currentLineIndex = tempLineIndex;
        }
        currentLineIndex++ ;
    }
    return errorFound;
}

/*check that there is no label declared as both entry and extern*/
int check_label_is_not_both_entry_and_extern(FILE *fPointer,char *fileName)
{
    char currentLine[256];
    char *currentWord = NULL;
    char currentLabelToCheck[30] = {0};
    int currentLineIndex = 1;
    int errorFound = FALSE;

    int lastLocation = 0;
    int tempLineIndex = 0;

    FOREVER
    {
        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;
        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*case 1: check if we get to entry first*/
        if(get_instruction_value(currentWord) == 1 && strlen(currentWord) <= 30)
        {
            /*We reached to entry, we will now hold the label and check if it had been declared also as extern*/
            currentWord = strtok(NULL, "\n");
            /*Hold the current label*/
            strcpy(currentLabelToCheck, currentWord);

            /*remember the last location we stop at*/
            lastLocation = ftell(fPointer);

            tempLineIndex = currentLineIndex;
            /*comparing process*/
            FOREVER
            {
                /*Hold the current line from file*/
                fgets(currentLine, 256, fPointer);
                currentLineIndex++;
                /*Check if we get to the end of the file*/
                if(feof(fPointer))
                {
                    fseek(fPointer, lastLocation, SEEK_SET);
                    break;
                }
                /*Hold the first word in line*/
                currentWord = strtok(currentLine, " ");
                /*check if we reached to extern*/
                if(get_instruction_value(currentWord) == 2)
                {
                    /*We reached to extern, we will hold the label follow*/
                    currentWord = strtok(NULL, "\n");
                    if(strcmp(currentLabelToCheck, currentWord) == 0)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | Error 13: The label '%s' was declared as entry and extern, its not allow.", fileName, currentWord);
                        printf("\nfirst appears as entry in line: %d\nsecond appears as extern in line: %d\n", tempLineIndex, currentLineIndex);
                        fseek(fPointer, lastLocation, SEEK_SET);
                        break;
                    }
                }
            }
            currentLineIndex = tempLineIndex;
        }
        /*case 2: check if we get to extern*/
        else if(get_instruction_value(currentWord) == 2 && strlen(currentWord) <= 30)
        {
            /*We reached to extern, we will now hold the label and check if it had been declared also as entry*/
            currentWord = strtok(NULL, "\n");
            /*Hold the current label*/
            strcpy(currentLabelToCheck, currentWord);

            /*remember the last location we stop at*/
            lastLocation = ftell(fPointer);

            tempLineIndex = currentLineIndex;
            /*comparing process*/
            FOREVER
            {
                /*Hold the current line from file*/
                fgets(currentLine, 256, fPointer);
                currentLineIndex++;
                /*Check if we get to the end of the file*/
                if(feof(fPointer))
                {
                    fseek(fPointer, lastLocation, SEEK_SET);
                    break;
                }
                /*Hold the first word in line*/
                currentWord = strtok(currentLine, " ");
                /*check if we reached to entry*/
                if(get_instruction_value(currentWord) == 1)
                {
                    /*We reached to entry, we will hold the label follow*/
                    currentWord = strtok(NULL, "\n");
                    if(strcmp(currentLabelToCheck, currentWord) == 0)
                    {
                        errorFound = TRUE;
                        printf("\nfile: %s | Error 13: The label '%s' was declared as entry and extern, its not allow.", fileName, currentWord);
                        printf("\nfirst appears as extern in line: %d\nsecond appears as entry in line: %d\n", tempLineIndex, currentLineIndex);
                        fseek(fPointer, lastLocation, SEEK_SET);
                        break;
                    }
                }
            }
            currentLineIndex = tempLineIndex;
        }
        currentLineIndex++ ;
    }
    return errorFound;
}

/*This program supports integers only, checking that all numbers are integers*/
int check_all_numbers_are_integers(FILE* fPointer,char *fileName)
{
    char currentLine[256];
    char *currentWord;
    int currentLineIndex = 1;
    int i,counter= 0, j=0 ;
    int flag = 0;
    int errorFound = FALSE;


    FOREVER {
        /*Check if we get to the end of the file*/
        if (feof(fPointer))
            break;

        /*Hold the current line from file*/
        fgets(currentLine, 256, fPointer);
        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

        /*We will go over the entire line, word by word*/
        while (currentWord != NULL) {
            /*If the current word is blanked space move on to next word*/
            if (strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0) {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*If the current line is comment, move on to next line*/
            if (currentWord[0] == ';') {
                break;
            }


            /*If the current word is symbol, move on to next word*/
            if (currentWord[strlen(currentWord) - 1] == ':') {
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*Fix the word*/
            if (currentWord[strlen(currentWord) - 1] == '\n')
                currentWord[strlen(currentWord) - 1] = '\0';

            if (currentWord[0] == '.') {
                currentWord[0] = ' ';
                j++;
            }


            if(currentWord[0] == ','){
                currentWord[0] = ' ';
                j++;
            }

            if ( strstr(currentWord, "data") || strstr(currentWord, "struct")) {
                flag = TRUE;
            }

            if(currentWord[0] == '#')
                flag = 1;


            if (flag) {
                for (i = 0; i < strlen(currentWord); i++) {
                    if (isdigit(currentWord[i]) != 0 && currentWord[i + 1] == '.') {
                        printf("\nfile: %s | line: %d | Error 14: Illegal declaration of number, number must be an integer.\n", fileName, currentLineIndex);
                        errorFound = TRUE;
                    }
                }
            }


            currentWord= strtok(NULL, " ");
            counter++;

        }

        currentLineIndex++ ;
        counter=0;
        flag = FALSE;

    }
    return errorFound;
}
