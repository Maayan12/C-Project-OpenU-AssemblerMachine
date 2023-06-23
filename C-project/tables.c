#include "assembler.h"

extern int ICF;
extern int DC;
extern Node* headLabels;

word dataTable[MAX_DATA_LEN]={0};
instruction instructionTable[MAX_DATA_LEN]={0};
action actions[] = {{"mov", 0, 0},
                    {"cmp", 1, 0},
                    {"add", 2, 10},
                    {"sub", 2, 11},
                    {"lea", 4, 0},
                    {"clr", 5, 10},
                    {"not", 5, 11},
                    {"inc", 5, 12},
                    {"dec", 5, 13},
                    {"jmp", 9, 10},
                    {"bne", 9, 11},
                    {"jsr", 9, 12},
                    {"red", 12, 0},
                    {"prn", 13, 0},
                    {"rts", 14, 0},
                    {"stop", 15, 0}
};
int getBaseAddress(int value)
{
    return value-(value % 16);
}
void resetMemmory()
{
    int i;
    for(i = 0; (int)dataTable[i].val.A > 0 || (int)dataTable[i].val.R > 0 || (int)dataTable[i].val.E > 0 ; i++)
    {
        dataTable[i].val.A = 0;
        dataTable[i].val.R = 0;
        dataTable[i].val.E = 0;
        dataTable[i].val.nineteen = 0;
        dataTable[i].val.operand = 0;
    }
    for(i = 0; (int)instructionTable[i].W.val.A > 0 || (int)instructionTable[i].W.val.R > 0 || (int)instructionTable[i].W.val.E > 0 ; i++)
    {
        instructionTable[i].L = 0;
        instructionTable[i].W.val.A = 0;
        instructionTable[i].W.val.R = 0;
        instructionTable[i].W.val.E = 0;
        instructionTable[i].W.val.nineteen = 0;
        instructionTable[i].W.val.operand = 0;
    }
}

void add(char* symbol, int type,int value, int lineCounter, bool* isError)
{
    Node* current=headLabels;
    Node* newNode;
    newNode = (Node*)malloc(sizeof(Node));
    if(!newNode)
    {
        printf("Error. memory allocation failed\n");
        exit(1);
    }
    newNode->data=(Data*) malloc(sizeof(Data));
    if(!newNode->data)
    {
        printf("Error. memory allocation failed\n");
        exit(1);
    }
    strncpy(newNode->data->symbol,symbol,MAX_SIZE_LABEL);
    newNode->data->value=value;
    newNode->data->baseAddress= getBaseAddress(value);
    newNode->data->offset=newNode->data->value - newNode->data->baseAddress;
    newNode->data->attributes[0]=0;
    newNode->data->attributes[1]=0;
    newNode->data->attributes[2]=0;
    newNode->data->attributes[3]=0;
    newNode->data->attributes[type]=1;
    newNode->next=NULL;
    if(headLabels==NULL)
    {
        headLabels=newNode;
        return;
    }

    while(current->next!=NULL) {
        if (!strcmp(current->data->symbol,symbol) &&
            (current->data->attributes[3] && type==3)) {
            return;
        }
        if (!strcmp(current->data->symbol, symbol)) {
            printf("Error in declaring label: %s in line %d.label already exists\n",symbol, lineCounter);
            *isError = true;
        }
        current = current->next;
    }
    current->next=newNode;

}

void updateLabels()
{
    Node* current=headLabels;
    while(current!=NULL)
    {
        if(current->data->attributes[1])
        {
            current->data->value += ICF;
            current->data->baseAddress = getBaseAddress(current->data->value);
            current->data->offset = current->data->value - current->data->baseAddress;
        }
        current=current->next;
    }
}

void freeList()
{
    Node* current=headLabels;
    Node* next = NULL;
    if(current == NULL)
        return;
    next = current->next;
    while(current!=NULL)
    {
        next = current->next;
        free(current->data);
        free(current);
        current=next;
    }

}

