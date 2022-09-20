#include "assembler.h"

/*This function will add new nodes to the macro-list*/
void insert_macro(Macro** root,char name[],char content[],int start_pos,int end_pos)
{
    Macro *new_node = malloc(sizeof(Macro));
    Macro*curr;

    /* if allocation fail */
    if (new_node == NULL) {
        exit(1);
    }

    /* converting data */
    new_node->next = NULL;
    strcpy(new_node->name,name);
    strcpy(new_node->content,content);
    new_node->start_pos=start_pos;
    new_node->end_pos=end_pos;

    /* if this is the first value we add a value to the list */
    if(*root == NULL){
        *root=new_node;
        return;
    }

    /* run over the list until the end */
    curr = *root;
    while(curr->next != NULL){
        curr = curr->next;
    }
    /* inserting data to tail */
    curr->next = new_node;

}

/*This function will print macro table*/
void print_macro_table(Macro** root)
{
    Macro *temp = *root;

    if(temp == NULL)
    {
        printf("List is empty.");
        return;
    }
    else
        while (temp != NULL)
        {
            printf("name: %s\ncontent: \n%s\n%d\n%d\n", temp->name, temp->content, temp->start_pos,temp->end_pos);
            temp = temp->next;
        }
    free(temp);
}

/*This function adds a symbol to the end of the symbol table*/
void add_symbol_at_end(symbol** head, char *flabel, int fAddress, int fARE)
{
    symbol *newNode = malloc(sizeof(symbol));
    symbol *temp = *head;

    if (newNode == NULL)
        exit(1);

    strcpy(newNode->label, flabel);
    newNode->decimalAddress = fAddress;
    newNode->ARE = fARE;
    newNode->next = NULL;

    if (temp == NULL)
    {
        *head = newNode;
        return;
    }

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

/*This function checks whether the word received as an argument is in the symbol table*/
int check_if_word_in_symbol_table(char *currentWord, symbol *head)
{
    symbol *temp = head;
    if (temp == NULL)
        return -1;

    while (temp != NULL)
    {
        if (strcmp(currentWord, temp->label) == 0)
            return 0;
        temp = temp->next;
    }

    return -1;
}

/*This function prints the symbol table*/
void print_symbol_table(symbol *head)
{
    symbol *temp = head;

    if(head == NULL)
    {
        printf("List is empty.");
        return;
    }

    else
        while (temp != NULL)
        {
            printf("%s\t%d\t%d\n", temp->label, temp->decimalAddress, temp->ARE);
            temp = temp->next;
        }
}

/*This function adds a binary-word(node) to the end of the list of binary words*/
void add_binary_word_at_end(binaryNode **head, int fBinaryWord, int fAddress, char *label)
{
    binaryNode *newNode = malloc(sizeof(binaryNode));
    binaryNode *temp = *head;
    int index;

    if (newNode == NULL)
        exit(1);

    newNode->binaryWord = fBinaryWord;
    newNode->decimalAddress = fAddress;
    newNode->next = NULL;

    if(label != NULL)
    {
        for(index = 0; index < strlen(label); index++)
        {
            if (label[index] == '.')
            {
                newNode->label[index] = '\0';
                break;
            }
            newNode->label[index] = label[index];
        }
        newNode->label[index] = '\0';
    }

    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

/*This function returns the last binary-word(node) in the list*/
binaryNode* get_last_binary_word(binaryNode *head)
{
    binaryNode *temp = head;

    if (temp == NULL)
        return temp;
    else
    {
        while (temp->next != NULL)
            temp = temp->next;

        return temp;
    }
}

/*This function prints the binary words*/
void print_binary_words_list(binaryNode *head)
{
    binaryNode *temp = head;

    if(head == NULL)
    {
        printf("List is empty.");
        return;
    }

    else
        while (temp != NULL)
        {
            print_binary(temp->binaryWord);
            printf("\t%d\n", temp->decimalAddress);
            temp = temp->next;
        }
}

/*This function removes the last binary word from the list*/
void remove_last_binary_word(binaryNode **head)
{
    binaryNode *temp = *head;
    binaryNode *second_last = NULL;

    if (*head == NULL)
        return;

    /*Move on to last node*/
    while(temp->next != NULL)
    {
        second_last = temp;
        temp = temp->next;
    }

    second_last->next = NULL;
    free(temp);
}

/*This function frees the memory of the 'binaryNode' linked list*/
void deallocate_binary_words(binaryNode **head)
{
    binaryNode *currentNode = *head;
    binaryNode *temp;

    while (currentNode != NULL)
    {
        temp = currentNode;
        currentNode = currentNode->next;
        free(temp);
    }
    *head = NULL;
}

/*This function frees the memory of the 'symbol' linked list*/
void deallocate_symbol_table(symbol **head)
{
    symbol *currentNode = *head;
    symbol *temp;

    while (currentNode != NULL)
    {
        temp = currentNode;
        currentNode = currentNode->next;
        free(temp);
    }
    *head = NULL;
}

/*This function frees the memory of the 'Macro' linked list*/
void deallocate_macros(Macro **head)
{
    Macro *currentNode = *head;
    Macro *temp;

    while (currentNode != NULL)
    {
        temp = currentNode;
        currentNode = currentNode->next;
        free(temp);
    }
    *head = NULL;
}

