#include "assembler.h"

extern word* dataTable[MAX_DATA_LEN];
extern action actions[NUMBER_OF_ACTION_TYPES];
extern instruction instructionTable[MAX_DATA_LEN];
extern int lineCounter;

int IC;
int DC;
int ICF;
int DCF;
bool isError;
Node* headLabels;

void readSourceFile(char *arr)
{
    FILE* pRead = fopen(arr, "r");
    int last = 0;
    char line [MAX_SIZE_LEN] = {0};
    IC = 100;
    DC = 0;
    ICF = 100;
    DCF = 0;
    isError = false;
    headLabels = NULL;
    lineCounter = 0;
    if(pRead == NULL)
    {
        printf("ERROR-opening file\n");
        return;
    }
    resetMemmory();
    while(fgets(line, MAX_SIZE_LEN, pRead) != NULL )/*receiving the line from the file*/
    {
        lineCounter++;
        last = (int)strcspn(line, "\n");
        line[last] = '\0';
        codeLine(line);
    }
    fclose(pRead);
    ICF=IC;
    DCF=DC;
    updateLabels();
    secondPassReadSourceFile(arr);
}

void codeLine(char *line)
{
    if(ifEntry(line) || ifExtern(line) || ifDefiningLabel(line))
    {
        return;
    }
    if(codeData(line))
    {
        return;
    }
    if(codeInstruction(line))
    {
        return;
    }
}

bool codeInstruction(char line[MAX_SIZE_LEN])
{
    char func [SIZE_FUNCT_NAME]={0};
    int actionIndex=0;
    int current=0;
    char* token =NULL;
    token = strtok(line, WHITE_CHARS);
    strncpy(func, token, 5);
    actionIndex = getActionIndex(func);
    if(actionIndex==-1)
    {
        /*didn't find the function in the table*/
        printf("Error in line: %d. not a valid line.(the function's name is incorrect or the data type - .data,.string is incorrect)\n", lineCounter);
        printf("%s\n",line);
        isError=true;
        return false;
    }
    /* calculating L*/
    current = IC;
    token = strtok(NULL, "\0");
    setWords(actionIndex, token);/*sets the words*/
    instructionTable[current-IC_SET].L = IC - current;
    return true;
}

int getImmediateAddressOperand(char* token)
{
    int num=0;
    token++;
    num= getNum(token,(int)strlen(token));
    if(num==INT_MAX)
    {
        printf("Error in line %d. not valid argument\n",lineCounter);
        isError=true;
    }

    if(num<0)
    {
        /* changing num by the two's complement*/
        num*=-1;
        num=~num+1;
    }

    return num;
}

int getNum(char arr [],int size)
{
    int num=0;
    int sign =0;/*  1=+  2=-  */
    int multiply=1;
    int i=0;
    for(i=size-1;i>=0;i--)
    {
        if(arr[i]=='+' && sign==0)
        {
            sign=1;
        }
        else if(arr[i]=='-' && sign==0)
        {
            sign=2;
        }
        else if(arr[i]>=ASCII_ZERO && arr[i]<=ASCII_NINE)
        {
            num += (arr[i] - ASCII_ZERO) * multiply;
            multiply *= 10;
        }
        else
        {
            return INT_MAX;
        }
    }
    if(sign==2)
    {
        return -num;
    }
    return num;
}

bool isRegister(char* isRegister)
{
    bool isItRegister = !strcmp(isRegister, "r0") ||
                        !strcmp(isRegister, "r1") ||
                        !strcmp(isRegister, "r2") ||
                        !strcmp(isRegister, "r3") ||
                        !strcmp(isRegister, "r4") ||
                        !strcmp(isRegister, "r5") ||
                        !strcmp(isRegister, "r6") ||
                        !strcmp(isRegister, "r7") ||
                        !strcmp(isRegister, "r8") ||
                        !strcmp(isRegister, "r9") ||
                        !strcmp(isRegister, "r10") ||
                        !strcmp(isRegister, "r11") ||
                        !strcmp(isRegister, "r12") ||
                        !strcmp(isRegister, "r13") ||
                        !strcmp(isRegister, "r14") ||
                        !strcmp(isRegister, "r15");
    return isItRegister;
}

bool immediateAddress(bool opcodeCondition, int* address, int* operand,char* token)
{
    if(token[0]=='#' && !opcodeCondition)/* addressing mode is not allowed in this operand*/
    {
        printf("Error in line %d. not valid addressing mode\n",lineCounter);
        isError=true;
        return true;
    }
    if(token[0]=='#' && opcodeCondition)
    {
        *address=IMMEDIATE_ADDRESS;
        *operand=getImmediateAddressOperand(token);
        return true;
    }
    return false;
}

bool registerDirect(bool opcodeCondition, int* address, int* operand,char* token)
{
    if(isRegister(token) && !opcodeCondition)/* addressing mode is not allowed in this operand*/
    {
        printf("Error in line %d. not valid addressing mode\n",lineCounter);
        isError=true;
        return true;
    }
    if(isRegister(token) && opcodeCondition)
    {
        *address=REGISTER_DIRECT;
        token++;
        *operand=atoi(token);
        return true;
    }
    return false;
}

bool addressingIndex(bool opcodeCondition, int* address, int* operand,char* token)
{
    if(strstr(token,"[") && !opcodeCondition)/* addressing mode is not allowed in this operand*/
    {
        printf("Error in line %d. not valid addressing mode\n",lineCounter);
        isError=true;
        return true;
    }
    if(strstr(token,"[") && opcodeCondition)
    {
        *address=ADDRESSING_INDEX;
        token+=strcspn(token,"[")+2;
        *operand=atoi(token);
        return true;
    }
    return false;
}

void isDirect(bool opcodeCondition, bool isImmediate, bool isRegisterDirect, bool isAddressingIndex, int* address, int* operand)
{
    if(!(isImmediate || isRegisterDirect || isAddressingIndex) && !opcodeCondition)/* addressing mode is not allowed in this operand*/
    {
        printf("Error in line %d. not valid addressing mode\n",lineCounter);
        isError=true;
        return;
    }
    if(!(isImmediate || isRegisterDirect || isAddressingIndex) && opcodeCondition)
    {
        *address=DIRECT_ADDRESSING;
        *operand=0;
    }
}

void setWords(int actionIndex, char *token)
{
    int targetAddress=0,targetRegister=0,sourceAddress=0,sourceRegister=0,funct=actions[actionIndex].funct, val1=0,val2=0;
    bool isImmediate=false, isRegisterDirect=false, isAddressingIndex=false;
    setFirstWord(actionIndex);
    if(actions[actionIndex].opcode<5)/* only if has a source operand*/
    {
        token = strtok(token,", \t\r\v\n\f\0");
        isImmediate = immediateAddress(actions[actionIndex].opcode <= 2, &sourceAddress, &val1, token);
        isRegisterDirect = registerDirect(actions[actionIndex].opcode <= 2, &sourceAddress, &sourceRegister, token);
        isAddressingIndex = addressingIndex(actions[actionIndex].opcode <= 4, &sourceAddress, &sourceRegister, token);
        isDirect(actions[actionIndex].opcode <= 4, isImmediate, isRegisterDirect, isAddressingIndex, &sourceAddress,&sourceRegister);
        token = strtok(NULL,"\0");
    }
    if(actions[actionIndex].opcode<=13) /* has a target operand */
    {
        token = strtok(token, ", \t\r\v\n\f\0");
        isImmediate = false;
        isRegisterDirect = false;
        isAddressingIndex = false;
        isImmediate = immediateAddress((actions[actionIndex].opcode == 1 || actions[actionIndex].opcode == 13),&targetAddress, &val2, token);
        isRegisterDirect = registerDirect((actions[actionIndex].opcode <= 5 || actions[actionIndex].opcode == 13 ||actions[actionIndex].opcode == 12), &targetAddress, &targetRegister,token);
        isAddressingIndex = addressingIndex(actions[actionIndex].opcode <= 13, &targetAddress, &targetRegister,token);
        isDirect(actions[actionIndex].opcode <= 13, isImmediate, isRegisterDirect, isAddressingIndex,&targetAddress, &targetRegister);
        token = strtok(NULL,",\t\r\v\n\f\0");
    }
    else
    {
        token = strtok(token,",\t\r\v\n\f\0");
    }
    if(token!=NULL)
    {
        printf("Error in line %d. syntax error\n",lineCounter);
        isError=true;
        return;
    }
    if(actions[actionIndex].opcode>13)/* stop or rts only have the first word*/
    {
        return;
    }
    setSecondWord(targetAddress, targetRegister, sourceAddress, sourceRegister, funct);
    if(sourceAddress==IMMEDIATE_ADDRESS && actions[actionIndex].opcode<5)
    {
        /*if the operand is in immediate address we set the word for the operand*/
        setThirdWord(val1,false);
    }
    if(sourceAddress == DIRECT_ADDRESSING || sourceAddress == ADDRESSING_INDEX)
    {
        /*if the operand contains a label we increase IC by 2 */
        IC+=UPPER_IC;/*L++*/
    }
    if(targetAddress==IMMEDIATE_ADDRESS)
    {
        /*if the operand is in immediate address we set the word for the operand*/
        setThirdWord(val2,false);
    }
    if(targetAddress == DIRECT_ADDRESSING || targetAddress == ADDRESSING_INDEX)
    {
        /*if the operand contains a label we increase IC by 2 */
        IC+=UPPER_IC;/*L++*/
    }
}

int getActionIndex(char func[SIZE_FUNCT_NAME])
{
    int i=0;
    for(i=0; i<=15;i++)
    {
        if (!strcmp(actions[i].name, func))
        {
            return i;
        }
    }
    return -1;
}

bool codeData(char line[MAX_SIZE_LEN])
{
    int i=0;
    int data = 0;
    char dataType [SIZE_data_NAME]={0};
    char dataString [MAX_SIZE_LEN]={0};
    char* token=NULL;
    char lineTemp[MAX_SIZE_LEN] = {0};
    strncpy(lineTemp,line,MAX_SIZE_LEN);
    token=strtok(lineTemp, WHITE_CHARS);
    if(token[0]!='.')   /* can't be a data line*/
    {
        return false;
    }
    token++;
    strncpy(dataType,token,SIZE_data_NAME);
    if(!strcmp(dataType, "string")) /* if it's a string*/
    {
        /* checking if the line is valid*/
        token=strtok(NULL, "\0");
        if(strcspn(token, "\"")>=strlen(token))
        {
            printf("Error in line %d. not valid .string declaration\n", lineCounter);
        }
        token+=strspn(token, WHITE_CHARS);
        if(token[0]!='\"' || token[1]=='\"')
        {
            printf("Error in line %d. not valid .string declaration\n", lineCounter);
        }
        token++;
        if(strcspn(token, "\"")>=strlen(token))
        {
            printf("Error in line %d. not valid .string declaration\n", lineCounter);
        }
        token=strtok(token, "\"");

        strcpy(dataString, token);
        token=strtok(NULL, WHITE_CHARS);
        if(token!=NULL)
        {
            printf("Error in line %d. not valid .string declaration\n", lineCounter);
        }
        for(i=0;i<=strlen(dataString); i++)
        {
            setThirdWord(dataString[i],true);
        }
        return true;
    }
    else if(!strcmp(dataType, "data"))/* if its a string*/
    {
        while(token)
        {
            token = strtok(NULL," ,\t\r\v\n\f");
            if(token)
            {
                data=getNum(token, (int)strlen(token));
                if(data==INT_MAX)
                {
                    printf("Error in line %d. not valid data (not a number)\n", lineCounter);
                    isError=true;
                    return true;
                }
                setThirdWord(data,true);

            }
        }
        return true;
    }
    else
    {
        return false;
        /* not a data line*/
    }

}

void checkLabelsName(char name[MAX_SIZE_LABEL]) {
    int i=0;
    if(!isalpha(name[0])) /*name does not start with a letter*/
    {
        isError=true;
        printf("Error in line %d. label name is not valid\n", lineCounter);
        return;
    }
    for(i=1;i<strlen(name);i++)
    {
        if(!isalnum(name[i]))/* not a letter or a number*/
        {
            isError=true;
            printf("Error in line %d. label name is not valid\n", lineCounter);
        }
    }
    if(isRegister(name))/* if has a name of a register*/
    {
        isError=true;
        printf("Error in line %d. label name is not valid- it ia a register's name\n", lineCounter);
    }
    if(strlen(name)<SIZE_FUNCT_NAME && getActionIndex(name)!=-1)/* if has a name of a funct*/
    {
        isError=true;
        printf("Error in line %d. label name is not valid- it ia a function's name\n", lineCounter);
    }
}

bool ifDefiningLabel(char line[MAX_SIZE_LEN])
{
    char labelName [MAX_SIZE_LABEL]= {0};
    char* token = NULL;
    int length = strlen(line);
    char lineTemp[MAX_SIZE_LEN] = {0};
    int tempIC=IC;
    int tempDC=DC;
    strncpy(lineTemp,line,MAX_SIZE_LEN);
    token = strtok(lineTemp, ":");
    if(strlen(token) == length)
    {
        /* not defining a label*/
        return false;
    }

    strcpy(labelName, token + strspn(token, WHITE_CHARS));
    checkLabelsName(labelName);

    token = strtok(NULL, "\0");
    if(codeData(token))/*if ia a data line */
    {
        add(labelName,1,tempDC,lineCounter, &isError);
    }
    else if (codeInstruction(token))/*if ia an instruction (code) line */
    {
        add(labelName,0,tempIC,lineCounter, &isError);
    }
    return true;
}

bool ifEntry(char line[MAX_SIZE_LEN])
{
    char lineTemp[MAX_SIZE_LEN] = {0};
    char* token = NULL;
    char labelName [MAX_SIZE_LABEL]= {0};
    strncpy(lineTemp,line,MAX_SIZE_LEN);
    token = strtok(lineTemp, WHITE_CHARS);
    if(strcmp(token, ".entry") != 0)
    {
        /* not defining an entry label*/
        return false;
    }
    token = strtok(NULL, WHITE_CHARS);
    if(token==NULL)
    {
        /* missing name of label*/
        printf("Error in line: %d. missing label\n",lineCounter);
        isError=true;
        return true;
    }
    strcpy(labelName, token);
    checkLabelsName(labelName);
    token = strtok(NULL, "\0");
    if(token!=NULL)
    {
        /*more text than necessary */
        printf("Error in line: %d. syntax error\n",lineCounter);
        isError=true;
        return true;
    }
    return true;
}

bool ifExtern(char line[MAX_SIZE_LEN])
{
    char lineTemp[MAX_SIZE_LEN] = {0};
    char labelName [MAX_SIZE_LABEL]= {0};
    char* token = NULL;
    strcpy(lineTemp,line);
    token = strtok(lineTemp, WHITE_CHARS);
    if(strcmp(token, ".extern") != 0)
    {
        /* not defining an extern label*/
        return false;
    }
    token = strtok(NULL, WHITE_CHARS);
    if(token==NULL)
    {
        /* missing name of label*/
        printf("Error in line: %d. missing label\n",lineCounter);
        isError=true;
        return true;
    }
    strncpy(labelName, token,MAX_SIZE_LABEL);
    checkLabelsName(labelName);
    token = strtok(NULL, WHITE_CHARS);
    if(token!=NULL)
    {
        /*more text than necessary */
        printf("Error in line: %d. syntax error\n",lineCounter);
        isError=true;
        return true;
    }
    add(labelName,3,0, lineCounter,&isError);
    return true;

}

