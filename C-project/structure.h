#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define MAX_SIZE_LEN 81
#define WHITE_CHARS " \t\r\v\n\f\0"
#define ASCII_ZERO '0'
#define ASCII_NINE '9'
#define MAX_SIZE_LABEL 32
#define IC_SET 100
#define MAX_DATA_LEN 8192
#define NUMBER_OF_ACTION_TYPES 16
#define SIZE_FUNCT_NAME 5
#define SIZE_data_NAME 7
#define IMMEDIATE_ADDRESS 0
#define DIRECT_ADDRESSING 1
#define ADDRESSING_INDEX 2
#define REGISTER_DIRECT 3
#define UPPER_IC 2

typedef struct word1
{
    unsigned int opcode:16;
    unsigned int E: 1;
    unsigned int R: 1;
    unsigned int A: 1;
    unsigned int nineteen: 1;
}word1;

typedef struct word2
{
    unsigned int targetAddress: 2;
    unsigned int targetRegister: 4;
    unsigned int sourceAddress: 2;
    unsigned int sourceRegister: 4;
    unsigned int funct: 4;
    unsigned int E: 1;
    unsigned int R: 1;
    unsigned int A: 1;
    unsigned int nineteen: 1;
}word2;


typedef struct word3
{
    unsigned int operand: 16;
    unsigned int E: 1;
    unsigned int R: 1;
    unsigned int A: 1;
    unsigned int nineteen: 1;
}word3;

typedef struct wordToPrint
{
    unsigned int E: 4;
    unsigned int D: 4;
    unsigned int C: 4;
    unsigned int B: 4;
    unsigned int A: 4;
}wordToPrint;

typedef union word{
    word1 w1;
    word2 w2;
    word3 val;
    wordToPrint wordToPrint;
}word;

typedef struct instruction
{
    word W;
    int L;
}instruction;

typedef struct action {
    char *name;
    int opcode;
    int funct;

}action;

typedef struct Data {
    char symbol [MAX_SIZE_LABEL];
    int value;
    int baseAddress;
    int offset;
    bool attributes[4]; /* [code, data, entry, extern]*/
}Data;

typedef struct Node {
    struct Data *data;
    struct Node *next;
}Node;

typedef struct MacroData
{
    char *name;
    char *content;
}MacroData;

typedef struct MacroNode
{
    struct MacroData *data;
    struct MacroNode *next;
}MacroNode;

