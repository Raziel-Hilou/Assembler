#include "assembler.h"

/*This function get as input a binary-word and updates the bits according to what it received as arguments from the first pass function.
 *Parameter 1: word- this is a node from the linked list 'binaryNode' that represent a binary word.
 *Parameter 2: operandType- this parameter represents whether the current word is a source operand or a destination operand.
 *Parameter 3: addressType- this parameter represents the ARE of the word
 */
void update_bits(binaryNode *word, char *operandType, int addressType)
{
    /*Binary word format: [9,8,7,6,5,4,3,2,1,0]. (This is by project definition)
     *Source operand field is: [5,4] bits.
     *destination operand field is: [3,2] bits*/
    int bitIndex;

    /*The current operand is a source operand we will update the bit field [5,4]*/
    if (strcmp("source", operandType) == 0)
    {
        switch (addressType)
        {
            case LABEL:
            {
                /*Turn on the bit in place 4*/
                bitIndex = 5;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;
            }
            case STRUCT:
            {
                /*Turn on the bit in place 5*/
                bitIndex = 6;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;
            }
            case REGISTER:
            {
                /*Turn on the bit in place 4 and in place 5*/
                bitIndex = 5;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                bitIndex = 6;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;
            }
        }
    }

    /*The current operand is a destination operand we will update the bit field [3,2]*/
    else if (strcmp("destination", operandType) == 0)
    {
        switch (addressType)
        {
            case LABEL:
            {
                /*Turn on the bit in place 2*/
                bitIndex = 3;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;

            }
            case STRUCT:
            {
                /*Turn on the bit in place 3*/
                bitIndex = 4;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;
            }
            case REGISTER:
            {
                /*Turn on the bit in place 2 and in place 3*/
                bitIndex = 3;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                bitIndex = 4;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;
            }
        }
    }
    else if (strcmp("ARE", operandType) == 0)
    {
        switch (addressType)
        {
            case EXTERNAL:
            {
                /*Turn on the bit in place 0*/
                bitIndex = 1;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;
            }
            case RELOCATABLE:
            {
                /*Turn on the bit in place 1*/
                bitIndex = 2;
                word->binaryWord = (word->binaryWord | 1 << (bitIndex - 1));
                break;
            }
        }
    }

    /*In this case 'register' is in source and destination operand*/
    else
    {
        bitIndex = get_register_value(operandType);
        word->binaryWord = word->binaryWord + (bitIndex << DESTINATION_OPERAND_FIELD);
    }
}

/*This function updates decimal address in 'data' linked-list.
 *We do this process because the data counter and the instruction counter are counted separately.
 *Parameter 1: dataList- the data linked-list.
 *Parameter 2: IC- the instruction counter.
 *Parameter 3: DC- the data counter.
 */
void update_address_in_data_list(binaryNode **dataList, int IC, int DC)
{
    binaryNode *temp = *dataList;
    int sumToAdd = IC-DC;

    if (temp == NULL)
        return;

    while(temp != NULL)
    {
        temp->decimalAddress += sumToAdd;
        temp = temp->next;
    }
}

/*This function performs the first pass.
 *In the first pass we build the symbol table and determine the address for each instruction.
 *Words with immediate or direct register,
 *already here we will update the required bits in each binary-word and the final value of the binary-word will be obtained.
 *binary-words that have a direct access address or a record access address,
 *their final value will be received only after the second pass.
 *However, we will keep a place in our memory for them.
 *Parameter 1: fpointer- the current file.
 *Parameter 2: placementIC- address in memory where we will store the number of instructions binary-words.
 *Parameter 3: placementDC- address in memory where we will store the number of data binary-words.
 *Parameter 4: placementSymbol- Address in memory of the symbol table so that we can access it later in our program.
 *Parameter 5: placementInstruction- Address in memory of the Instruction linked-list so that we can access it later in our program.
 *Parameter 6: placementData- Address in memory of the data linked-list so that we can access it later in our program.
 */
void first_pass(FILE* fpointer, int *placementIC, int *placementDC, symbol **placementSymbol, binaryNode **placementInstruction, binaryNode **placementData)
{
    /*Instructions counter*/
    int IC = 100;
    /*Data counter*/
    int DC = 0;

    /*This linked list is actually the table of symbols we will create*/
    symbol *label = NULL;
    /*These linked lists give us the overall picture of machine memory*/
    /*A linked list of instructions*/
    binaryNode *instruction = NULL;
    /*A linked list of data*/
    binaryNode *data = NULL;
    /*A temporary node so that we can make changes in the addresses of the last binary-word*/
    binaryNode *temp = NULL;

    char *currentWord;
    char currentLine[MAX_CHARS_IN_LINE];
    int i;

    /*There are different instructions/registers/opcodes*/
    int opCodeValue = -1, registerValue = -1, instructionValue = -1;

    /*There are different field-bits for source and destination operand, so we must distinguish between them.
     *'operandCounter' purpose is to make that distinction*/
    int operandCounter;
    int isDesAndSourOperand;

    int tempNumber;

    /*This variable is a flag, when the flag is True, the current word is a label*/
    int isLabel;

    FOREVER
    {
        /*Hold the current line in file*/
        fgets(currentLine, MAX_CHARS_IN_LINE, fpointer);
        /*Check if we get to the end of the file*/
        if(feof(fpointer))
            break;
        /*Hold the first word in line*/
        currentWord = strtok(currentLine, " ");

	/*Reset those for the next line*/
        operandCounter = 0;
        isDesAndSourOperand = 0;

        /*Run over current line (word-by-word)*/
        while (currentWord != NULL)
        {
            isLabel = TRUE;
            /*If the current word is blanked space move on to next word*/
            if(strcmp(currentWord, " ") == 0 || strcmp(currentWord, "\t") == 0 || strcmp(currentWord, "\n") == 0)
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

            /*If the current word is symbol, add it to the symbols-table.
             *Note: by project definition, a symbol always ends in the character ':'*/
            if(currentWord[strlen(currentWord) - 1] == ':')
            {
                /*Deleting the character ':' from the symbol, we do not need it*/
                currentWord[strlen(currentWord) - 1] = '\0';

                /*Add the symbol to the symbols-table.
                 * Note: there are 3 options to label. ()
                 * First option - the label was declared at the beginning of the file as entry.
                 * Second option - the label was declared at the beginning of the file as external.
                 * Third option - none of the above.*/
                if(label_was_declared_as_an_entry(currentWord, &label) == TRUE)
                    add_symbol_at_end(&label, currentWord, IC, ENTRY_ON);
                else
                    add_symbol_at_end(&label, currentWord, IC, 0);

                /*Move on to next word in line*/
                currentWord = strtok(NULL, " ");
                continue;
            }

            /*case 1: the current word is opcode*/
            opCodeValue = get_opCode_value(currentWord);
            if(opCodeValue != -1)
            {
                add_binary_word_at_end(&instruction, opCodeValue << OPCODE_FIELD, IC, NULL);
                temp = get_last_binary_word(instruction);
                IC++;

                /*Those opcodes don't have source address so we will move on immediately to destination address fields*/
                if(opCodeValue >= 7 && opCodeValue <= 15)
                    operandCounter++;

                /*Move on to next word in line*/
                currentWord = strtok(NULL, ", ");
                continue;
            }

            /*case 2: the current word is constant number operand*/
            /*Immediate address is '#' and its 00 in binary*/
            if(currentWord[0] == '#')
            {
                isLabel = FALSE;

                /*Convert string to int*/
                currentWord = currentWord+1; /*Step over char '#' we don't need it*/

                add_binary_word_at_end(&instruction, atoi(currentWord), IC, NULL);

                /*Distinguishing between a source operand and a destination operand, different bits field to update*/
                if (operandCounter == 0)
                    update_bits(temp, "source", 0);
                else if(operandCounter == 1)
                    update_bits(temp, "destination", 0);

                operandCounter++;
                IC++;
            }

            /*case 3: the current word is register operand*/
            registerValue = get_register_value(currentWord);
            if(registerValue != -1)
            {
                isLabel = FALSE;
                /*case 3.1: register is source operand*/
                if (operandCounter == 0)
                {
                    add_binary_word_at_end(&instruction, registerValue << OPCODE_FIELD, IC, NULL);
                    update_bits(temp, "source", REGISTER);
                    operandCounter++;
                    IC++;
                    isDesAndSourOperand++;
                }

                /*case 3.2: register is destination operand*/
                else if (operandCounter == 1)
                {
                    add_binary_word_at_end(&instruction, registerValue << DESTINATION_OPERAND_FIELD, IC, NULL);
                    update_bits(temp, "destination", REGISTER);
                    operandCounter++;
                    IC++;
                    isDesAndSourOperand++;
                }

                /*case 3.3: destination and source is register operand*/
                if(isDesAndSourOperand == 2)
                {
                    remove_last_binary_word(&instruction);
                    update_bits(get_last_binary_word(instruction) , currentWord, REGISTER_IN_SOURCE_AND_DESTINATION);
                    IC--;
                }


            }

            /*case 4: the current word is instruction word.
             *That is, the word is: ".string" or ".data" or ".struct" or ".entry" or ".extern"*/
            instructionValue = get_instruction_value(currentWord);
            if (instructionValue != -1)
            {
                isLabel = FALSE;
                switch (instructionValue)
                {
                    /*The current word is ".entry"*/
                    case 1:
                    {
                        /*Move on to next word, it is symbol*/
                        currentWord = strtok(NULL, " \n");
                        /*Add the symbol to the symbols-table*/
                        add_symbol_at_end(&label, currentWord, 0, ENTRY);
                        currentWord = strtok(NULL, " ");
                        break;
                    }
                    /*The current word is ".extern"*/
                    case 2:
                    {
                        /*Move on to next word, it is symbol*/
                        currentWord = strtok(NULL, " \n");
                        /*Add the symbol to the symbols-table*/
                        add_symbol_at_end(&label, currentWord, 0, EXTERN);
                        currentWord = strtok(NULL, " ");
                        break;
                    }
                    /*The current word is ".data"*/
                    case 3:
                    {
                        /*Move on to next word, it is data*/
                        currentWord = strtok(NULL, ",");

                        /*Run over current line (word-by-word)*/
                        while (currentWord != NULL)
                        {
                            /*Convert string to int*/
                            tempNumber = atoi(currentWord);

                            /*Add the data to the data linked-list*/
                            add_binary_word_at_end(&data, tempNumber, DC, NULL);

                            /*Move on to next data*/
                            currentWord = strtok(NULL, ",");
                            IC++;
                            DC++;
                        }
                        break;
                    }
                    /*The current word is ".string"*/
                    case 4:
                    {
                        /*Move on to next word, it is the string*/
                        currentWord = strtok(NULL, " \"");
                        /*Run over the string and adding each char to data linked-list*/
                        for(i = 0; i < strlen(currentWord); i++)
                        {
                            add_binary_word_at_end(&data, currentWord[i], DC, NULL);
                            IC++;
                            DC++;
                        }

                        /*Adding word '\0' which represent the end of the string, '\0' its 0 in binary*/
                        add_binary_word_at_end(&data, 0, DC, NULL);
                        IC++;
                        DC++;
                        break;
                    }
                    /*The current word is ".struct"*/
                    case 5:
                    {
                        /*Move on to next word, it is number
                         *The first field in struct is number*/
                        currentWord = strtok(NULL, ",");

                        /*Convert string to int*/
                        tempNumber = atoi(currentWord);
                        /*Add the data to the data linked-list*/
                        add_binary_word_at_end(&data, tempNumber, DC, NULL);

                        IC++;
                        DC++;

                        /*Move on to next word, it is string
                         *The second field in struct is string*/
                        currentWord = strtok(NULL, " \"");

                        /*Run over the string and adding each char to data linked-list*/
                        for(i = 0; i < strlen(currentWord); i++)
                        {
                            add_binary_word_at_end(&data, currentWord[i], DC, NULL);
                            IC++;
                            DC++;
                        }

                        /*Adding word '\0' which represent the end of the string, '\0' its 0 in binary*/
                        add_binary_word_at_end(&data, 0, DC, NULL);
                        IC++;
                        DC++;
                        break;
                    }
                    default: break;
                }
            }

            /*case 5: the current word is struct*/
            if(currentWord != NULL && currentWord[strlen(currentWord) - 2] == '.')
            {
                isLabel = FALSE;
                /*The binary word we are now adding is the address of current struct, we will know the address only later*/
                add_binary_word_at_end(&instruction, 0, IC, currentWord);
		/*Update the bits at 'ARE' field (we will know the all address only later at second pass)*/
                update_bits(get_last_binary_word(instruction), "ARE", RELOCATABLE); 
                IC++;

                /*The binary word we are now adding is the field number in the current struct*/
                currentWord = &currentWord[strlen(currentWord) - 1];
                tempNumber = atoi(currentWord);
                add_binary_word_at_end(&instruction, tempNumber << 2, IC, NULL);

                if(operandCounter == 0)
                    update_bits(temp, "source", STRUCT);
                else
                    update_bits(temp, "destination", STRUCT);

                operandCounter++;
                IC++;
            }

            /*case 6: the current word is external label*/
            if(check_if_extern_label(label, currentWord) == 0)
            {
                isLabel = FALSE;

                add_binary_word_at_end(&instruction, 0, IC, currentWord);
                update_bits(get_last_binary_word(instruction), "ARE", EXTERNAL);

                if(operandCounter == 0)
                    update_bits(temp, "source", LABEL);
                else
                    update_bits(temp, "destination", LABEL);

                operandCounter++;
                IC++;
            }

            /*case 7: the current word is label.
             *This label has been declared or will be declared later.
             *Therefore we will reserve a place for her, in binary-words linked-list, but we will only know her address at the end of the entire first passage*/
            if (isLabel == TRUE)
            {
                add_binary_word_at_end(&instruction, 0, IC, currentWord);
                update_bits(get_last_binary_word(instruction), "ARE", RELOCATABLE);

                if(operandCounter == 0)
                    update_bits(temp, "source", LABEL);
                else
                    update_bits(temp, "destination", LABEL);

                operandCounter++;
                IC++;
            }

            /*Move on to next word in line*/
            if (operandCounter == 1)
                currentWord = strtok(NULL, ", ");
            else
                currentWord = strtok(NULL, " ");
        }
    }

    /*Update address in data-list*/
    update_address_in_data_list(&data, IC, DC);
    /*Placing these variables so that we can work with them later outside the current function*/
    *placementIC = IC;
    *placementDC = DC;
    *placementSymbol = label;
    *placementInstruction = instruction;
    *placementData = data;
}
