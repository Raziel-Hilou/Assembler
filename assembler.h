#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define FOREVER for(;;)
#define MAX_CHARS_IN_LINE 80
#define MAX_CHARS_IN_LABEL 30
#define NUMBER_OF_TESTS 14
#define MAX 80
#define FALSE 0
#define TRUE 1

/*Flags that help us distinguish between the entry cases and extern cases.
 *The main use is done in a source files: "firstPass", "auxiliartyFunctions".*/
#define ENTRY -1
#define EXTERN -2
#define EXTERN_ON 1
#define ENTRY_ON 2

/*Flags that help us distinguish between ARE types.
 *The main use is done in a source file: "auxiliartyFunctions" (in a specific function "update_bits").*/
#define EXTERNAL 1
#define RELOCATABLE 2

/*Flags that help us distinguish between the different addresses*/
#define LABEL 1
#define STRUCT 2
#define REGISTER 3

/*Flags that help us distinguish between different bits field.
 *The main use is done in a source files: "firstPass", "auxiliartyFunctions" (in a specific function "update_bits").*/
#define OPCODE_FIELD 6
#define SOURCE_OPERAND_FIELD 4
#define DESTINATION_OPERAND_FIELD 2

/*A flag indicating a special case where both the source operand and the destination operand are registers.
 *The main use is done in a source files: "firstPass", "auxiliartyFunctions" (in a specific function "update_bits")*/
#define REGISTER_IN_SOURCE_AND_DESTINATION -3

/*A node in our linked list will represent a Macro. And the linked list, represent the entire Macros in the file*/
typedef struct Macro {
    char name[MAX];
    char content[MAX];
    int start_pos;
    int end_pos;
    struct Macro* next;
}Macro;

/*A node represent a label in the labels table. And the linked list, represent the entire table*/
typedef struct symbol{
    char label[MAX_CHARS_IN_LABEL];
    int decimalAddress;
    int ARE;
    struct symbol* next;
}symbol;

/*Instructions / Data, linked-list*/
typedef struct binaryNode{
    int binaryWord;
    int decimalAddress;
    char label[MAX_CHARS_IN_LABEL];
    struct binaryNode* next;
}binaryNode;

/*A group of words with a numerical value for each word.
 *Useful for cases where we need to diagnose between registers or between opCodes, and more examples.*/
typedef struct {char *word; int x;}group;

/*Declare prototype functions from 'pre_processor.c' source file*/
int macro_or_endmacro(char*);
void scan_content(FILE*, Macro**);
int check_for_macro_name(Macro*, char*);
FILE* rewrite_file(FILE*, Macro**, char*);
FILE* macro_retirement(FILE *fPointer, char*);

/*Declare prototype functions from 'validationTests.c' source file*/
int check_max_chars_in_line(FILE*, char*);
int check_opCode_exist(FILE*, char*);
int check_number_of_operands(FILE*, char*);
int check_operand_type(FILE*, char*);
int check_for_double_opcode(FILE*, char*);
int check_semicolon_appears_in_beginning_of_line(FILE*, char*);
int check_labels_is_not_reserved_words_in_assembly(FILE*, char*);
int check_declaration_of_constant_operand(FILE*, char*);
int check_max_30_chars_in_label(FILE*,char*);
int check_the_file_name_is_correct(FILE*,char*);
int check_the_register_name_is_correct(FILE*,char*);
int check_for_double_label(FILE*, char*);
int check_label_is_not_both_entry_and_extern(FILE*,char*);
int check_all_numbers_are_integers(FILE*,char*);

/*Declare prototype functions from 'linked_list_functions.c' source file*/
void insert_macro(Macro**, char*, char*, int, int);
void print_macro_table(Macro**);
void add_symbol_at_end(symbol**, char*, int, int);
void print_symbol_table(symbol*);
int check_if_word_in_symbol_table(char*, symbol*);
void add_binary_word_at_end(binaryNode**, int, int, char*);
void print_binary_words_list(binaryNode*);
binaryNode* get_last_binary_word(binaryNode*);
void remove_last_binary_word(binaryNode**);
void deallocate_binary_words(binaryNode**);
void deallocate_symbol_table(symbol**);
void deallocate_macros(Macro**);

/*Declare prototype functions from 'auxiliaryFunctions.c' source file*/
void delete_space_tab(FILE*);
int get_opCode_value(char*);
int get_register_value(char*);
void print_binary(int);
int get_instruction_value(char*);
int check_if_extern_label(symbol* , char*);
int existing_entry_label(symbol**);
int existing_external_label(symbol**);
int label_was_declared_as_an_entry(char*, symbol**);
int label_was_declared_as_an_extern(char*, symbol**);
int check_blanked_space(char*);
void delete_space_tabs_in_file(FILE*);

/*Declare prototype functions from 'firstPass.c' source file*/
void first_pass(FILE*, int*, int*, symbol**, binaryNode**, binaryNode**);
void update_address_in_data_list(binaryNode**, int, int);
void update_bits(binaryNode*, char*, int);

/*Declare prototype functions from 'secondPass.c' source file*/
void second_pass(binaryNode**, symbol**);
void update_address(binaryNode*, symbol**);

/*Declare prototype functions from 'createOutputFiles.c' source file*/
void create_object_file(char*, binaryNode**, binaryNode**, int, int);
void create_externals_file(char*, symbol**, binaryNode**);
void create_entries_file(char*, symbol**);
char *convert_to_Base32(int, int, int);
void fixBase32Value(char*, int);
void print_to_file_object(FILE*, binaryNode**, binaryNode**);
void print_to_file_externals_labels(FILE*, binaryNode**);
void print_to_file_entries_labels(FILE*, symbol**);
int two_complement(int);
int current_binary_word_is_extern_label(int);
void change_ending_file_name(char*);