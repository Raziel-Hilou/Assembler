#include "assembler.h"

/*This function will check if line contains a macro/endmacro call
 *Parameter 1: line- the current line from input file.
 */
int macro_or_endmacro(char line[]) {
    char macro[] = "macro";
    char endmacro[] = "endmacro";
    int i = 0, j = 0;


    /* ignoring spaces*/
    while (line[i] == ' ') {
        i++;
    }

    /*comparing first word in line to "macro"*/
    while (line[i] == macro[j]) {
        i++;
        j++;
    }

    /*macro call found*/
    if (j == 5) {
        return 1;
    }
    /*endmacro call found*/
    if (strstr(line,endmacro)){
        return 2;
    }

    else {
        return 0;
    }
}


/*This function will scan the text and detect macro declarations and will create the macro table. 
 *Parameter 1: fPointer- the current file.
 *Parameter 2: root- the macro table(linked-list).
 **/
void scan_content(FILE* fpointer,Macro** root)
 {
    int n, i, content_counter = 0, content_size = 10, scan_name = 0, start_pos, end_pos, text_pos = 0, flag = 0;
    char buffer[MAX_CHARS_IN_LINE], macro_name[MAX_CHARS_IN_LINE],content[MAX_CHARS_IN_LINE];
    fseek(fpointer, 0, SEEK_SET);
    memset(macro_name, '\0', MAX_CHARS_IN_LINE);
    memset(buffer, '\0', MAX_CHARS_IN_LINE);
    memset(content,'\0',MAX_CHARS_IN_LINE);


    /* the following loop will scan the file line by line. in each line it will check for macro.
     if macro declaration was found, an internal loop will save macro info and will store it in macro table*/
    while (fgets(buffer, MAX_CHARS_IN_LINE, fpointer) != NULL) {
        n = (int) strlen(buffer);
        text_pos += n;
        i = 0;
	
        /* if macro declaration was finished, keep search for another one. */
        if (strstr(buffer, "endmacro")) {
            end_pos = text_pos;

            /* cleans extra cells created due to reallocation */
            for(i=content_counter; i<=content_size ; i++){
                content[i]='\0';
            }

            insert_macro(root,macro_name,content,start_pos,end_pos);

            /* set data to proceed */
            memset(content,'\0',MAX_CHARS_IN_LINE);
            memset(macro_name, '\0', MAX_CHARS_IN_LINE);
            i=0;
            flag = 0;
            content_counter = 0;
            content_size=0;
            start_pos=0;
            scan_name=0;
        }

        /* if declaration hasn't finished, keep inserting content */
        if (flag == 1) {
            while (i < n-1) {

                content[content_counter] = buffer[i];
                content_counter++;
                i++;
            }
        }



        /* macro declaration began */
        if (macro_or_endmacro(buffer) == 1) {
            while (i < n) {
                /* ignoring spaces*/
                while (buffer[i] == ' ') {
                    i++;
                }

                /*skip "macro" */
                while (buffer[i] != ' ') {
                    i++;
                }
                /* ignoring spaces*/
                while (buffer[i] == ' ') {
                    i++;
                    /*printf("%d\n",i);*/
                }

                /* scanning macro's name*/
                while (1) {
                    for( ; i<=n ; i++){
                        macro_name[scan_name] = buffer[i];
                        scan_name++;
                    }
                    /* puts(macro_name) */
                    start_pos = text_pos - n + 1;
                    flag = 1;
                    break;
                }

            }

        }
    }
}

/*This function receives a line from the current file and checks whether there is a macro call in this line
 *Parameter 1: node- macro-table's node.
 *Parameter 2: line- the current line from file.
 *
 *return TRUE: if we found a macro call.
 *return FALSE: if have'nt found a macro call.
 */
int check_for_macro_name(Macro* node,char* line)
{
    Macro* temp = node;
    int i=0,j=0,name_length=(int)strlen(temp->name);

    if (temp==NULL){
        puts("error - list is empty");
        exit(1);
    }

    /*ignore space*/
    while(line[i] == ' ')
    {
        i++;
    }

    /*comparing first word to macro name char by char*/
    while(line[i]!=' ' && line[i]!='\0' && line[i]==temp->name[j])
    {
        i++;
        j++;

    }

    /*if line is equal to node name*/
    if(j==name_length ){
        return TRUE;
    }
    /*macro call in last line*/
    if(j==name_length-1 && line[strlen(line)] == '\0')
        return TRUE;


    else
        return FALSE;
}



/*This function will get the text after macros were cleaned and copy it neatly to a new file
 *Parameter 1: inputFile- the current file.
 *Parameter 2: head- macro-table (linked-list).
 *Patameter 3: fileName - current file name.
 **/
FILE* rewrite_file(FILE *inputFile, Macro **head, char *fileName)
{
    FILE *newFile;
    Macro *temp = *head;
    Macro *check;

    /*Declare a temporary variable for the file name, so that we do not change the original file name for the following outPut files*/
    char tempFileName[FILENAME_MAX];
    char *finalFileName = NULL;
    char currentLine[MAX_CHARS_IN_LINE] = {0};
    int line_length = 0,content_flag = 0, pos=0;

    strcpy(tempFileName, fileName);
    change_ending_file_name(tempFileName);

    /*Opening a new file for writing.
     *The name of the file is the original name + ".ob" (the required suffix).*/
    finalFileName = strcat(tempFileName, ".am");
    newFile = fopen(finalFileName, "w");

    /*scan line by line*/
    fseek(inputFile, 0, SEEK_SET);
    while (fgets(currentLine, MAX_CHARS_IN_LINE, inputFile) != NULL) {
        line_length = (int) strlen(currentLine);
        check = *head;
        pos += line_length;

        while (1) {
            if (pos == temp->start_pos - 1) {
                fseek(inputFile, temp->end_pos , SEEK_SET);
                pos += temp->end_pos - temp->start_pos + 1;
                temp = *head;
            }
            else
                temp = temp->next;

            if (temp == NULL){
                temp=*head;
                break;
            }
        }

        /*compare each line to macro name*/
        while (check != NULL) {

            if (check_for_macro_name(check, currentLine) == 1) {
                fputs(check->content, newFile);
		fputs("\n", newFile);
                content_flag = 1;
                break;
            }
            else
                check = check->next;
        }

        /*if match not found check for empty line*/
        if (!content_flag)
        {
            /*create empty line simulation and compare it to buffer, if equal, jump to next line*/

            fputs(currentLine, newFile);

        }

        content_flag=0;
        memset(currentLine, '\0', MAX_CHARS_IN_LINE);
    }

    fclose(inputFile);
    fclose(newFile);
    newFile = fopen(finalFileName, "r");
    return newFile;
}


/*This function initiate all functions in right order to reach desired result
 *Parameter 1: fPointer- the current file.
 *Parameter 2: fileName- the current file name.
 */
FILE* macro_retirement(FILE *fPointer, char *fileName)
{
    FILE *newFile;
    Macro *macrosLinkedList = NULL;

    /*Find all macros in file and link it to the linked-list*/
    scan_content(fPointer, &macrosLinkedList);

    if (macrosLinkedList == NULL)
        return fPointer;
    else
    {
        newFile = rewrite_file(fPointer, &macrosLinkedList, fileName);
	deallocate_macros(&macrosLinkedList);
        return newFile;
    }
}
