#include "assembler.h"

/*This function gets a binary word identified as having to contain a label address and updates it to contain the final address of the label.
 *Parameter 1: nodeToUpdate- the binary word we want to update.
 *Parameter 2: symbolTable- the symbol table linked-list.
 */
void update_address(binaryNode *nodeToUpdate, symbol **symbolTable)
{
    symbol *temp = *symbolTable;

    while (temp != NULL)
    {
        if(strcmp(temp->label, nodeToUpdate->label) == 0)
        {
            nodeToUpdate->binaryWord += (temp->decimalAddress<<2);
            return;
        }
        temp = temp->next;
    }
}

/*This function do the second pass and actually completes the final coding for all the labels whose address we still did not know in the first pass.
 *After this pass we have the final encoding of the source file received as input.
 *The coding is divided into 3 linked lists- dataList (contains all data in file), instructionList (contains all Instructions and guidelines in file), symbolTable.
 *Parameter 1: instructionList- the instruction linked-list.
 *Parameter 2: symbolTable- the symbol table linked-list.
 */
void second_pass(binaryNode **instructionList, symbol **symbolTable)
{
    binaryNode *currentNode = *instructionList;

    /*Run over all instruction*/
    while (currentNode != NULL)
    {
	/*If the 2nd bit is on then the current binary word is a label address and needs to be updated*/
        if (currentNode->binaryWord & 2)
        {
            update_address(currentNode, symbolTable);
        }
        currentNode = currentNode->next;
    }
}

