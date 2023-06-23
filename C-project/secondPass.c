#include "assembler.h"

extern word dataTable[MAX_DATA_LEN];
extern instruction instructionTable[MAX_DATA_LEN];
extern action actions[NUMBER_OF_ACTION_TYPES];
extern int ICF;
extern int DCF;
extern Node* headLabels;
extern int lineCounter;
extern bool isError;

int secondPassIC;
FILE* fpExt;

void secondPassReadSourceFile(char *fileName)
{
    FILE *fpRead = NULL;
    char line[MAX_SIZE_LEN] = {0};
    char* linePointer = 0;
    char labelName[31] = {0};
    int i;
    secondPassIC = 0;
    fpExt = NULL;
    if(isExtExist())
    {
        createExtFile(fileName, true);
    }
    lineCounter = 0;
    secondPassIC = IC_SET;
    fpRead = fopen(fileName, "r");
    if(fpRead == 0)
    {
        printf("ERROR-opening file\n");
        isError = true;
    }
    while(fgets(line, MAX_SIZE_LEN, fpRead))
    {
        lineCounter++;
        memset(labelName,0,sizeof(labelName));
        linePointer = line;
        linePointer += skipLabel(linePointer);
        if(!isSkip(linePointer))
        {
            if (isEntry(linePointer))
            {
                getEntryName(linePointer, labelName);
                if (!addEntry(labelName))
                {
                    printf("Error in line #%d: the label name \"%s\" is not exist\n",lineCounter,labelName);
                    isError = true;
                }
            }
            else
            {
                codeMissingPart(linePointer);
            }
        }
    }
    if(isError)
    {
        for(i = 0; fileName[i] != '.'; i++);
        fileName[i + 2] = 's';
        printf("The error above is in the file %s\n",fileName);
    }
    fclose(fpRead);
    if(isError == false)
    {
        createEntFile(fileName);
        createObFile(fileName);
    }
    if(isExtExist())
    {
        createExtFile(fileName, false);
    }
    freeList();
}

bool isExtExist()
{
    Node* current = headLabels;
    while(current != NULL)
    {
        if(current->data->attributes[3] == true)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool isSkip(char* line)
{
    char *token = NULL;
    char tempLine[MAX_SIZE_LEN] = {0};
    strcpy(tempLine,line);
    token = strtok(tempLine,WHITE_CHARS);
    if(strcmp(token,".extern") == 0 || strcmp(token,".string") == 0 || strcmp(token,".data") == 0)
    {
        return true;
    }
    return false;
}

bool isEntry(char* line)
{
    char *token = NULL;
    char tempLine[MAX_SIZE_LEN] = {0};
    strcpy(tempLine,line);
    token = strtok(tempLine,WHITE_CHARS);
    if(strcmp(token,".entry") == 0)
    {
        return true;
    }
    return false;
}

void getEntryName(char* line,char* name)
{
    char* token = NULL;
    char tempLine[MAX_SIZE_LEN] = {0};
    strcpy(tempLine,line);
    token = strtok(tempLine,WHITE_CHARS);
    token = strtok(NULL,WHITE_CHARS);
    strcpy(name,token);
}

bool addEntry(char* name)
{
    struct Node* current = headLabels;
    while(current != NULL)
    {
        if(strcmp(current->data->symbol,name) == 0)
        {
            current->data->attributes[2] = 1;
            return true;
        }
        current = current->next;
    }
    return false;
}

int whatAction(char* line)
{
    char tempLine[MAX_SIZE_LEN] = {0};
    char *token = NULL;
    int i;
    strcpy(tempLine,line);
    token = strtok(tempLine, WHITE_CHARS);
    for(i=0; i<=15;i++)
    {
        if (!strcmp(actions[i].name, token))
        {
            return actions[i].opcode;
        }
    }
    return -1;
}

void createObFile(char* fileName)
{
    FILE* fp = NULL;
    int i;
    char* obFileName = NULL;
    obFileName = (char*)malloc(sizeof(char)*strlen(fileName));
    if(obFileName == NULL)
    {
        printf("Error in line #%d: Memory allocation failed.\n",lineCounter);
        exit(1);
    }
    strcpy(obFileName,fileName);
    for(i = 0; obFileName[i-1] != '.';i++);
    obFileName[i] = 'o';
    obFileName[i+1] = 'b';
    fp = fopen(obFileName,"w");
    if(fp == NULL)
    {
        printf("Error in line #%d: Memory allocation failed.\n",lineCounter);
        exit(1);
    }
    fprintf(fp,"          %d %d\n", (ICF-IC_SET),DCF);
    for(i = 0; (int)instructionTable[i].W.val.A > 0 || (int)instructionTable[i].W.val.R > 0 || (int)instructionTable[i].W.val.E > 0 ; i++)
    {
        fprintf(fp," %04d A%x-B%x-C%x-D%x-E%x\n",(i + IC_SET), (int)instructionTable[i].W.wordToPrint.A , (int)instructionTable[i].W.wordToPrint.B , (int)instructionTable[i].W.wordToPrint.C , (int)instructionTable[i].W.wordToPrint.D , (int)instructionTable[i].W.wordToPrint.E );
    }
    for(i = 0; (int)dataTable[i].val.A > 0 || (int)dataTable[i].val.R > 0 || (int)dataTable[i].val.E > 0 ; i++)
    {
        fprintf(fp," %04d A%x-B%x-C%x-D%x-E%x\n", i + ICF , (int)dataTable[i].wordToPrint.A , (int)dataTable[i].wordToPrint.B , (int)dataTable[i].wordToPrint.C , (int)dataTable[i].wordToPrint.D , (int)dataTable[i].wordToPrint.E );
    }
    fclose(fp);
    free(obFileName);
}

void createEntFile(char* fileName)
{
    bool isEntExist = false;
    Node* current = headLabels;
    FILE* fp = NULL;
    int i;
    char* entFileName = NULL;
    while(current != NULL && current->next != NULL)
    {
        if(current->data->attributes[2] == true)
        {
            if(isEntExist == false)
            {
                entFileName = (char*)malloc(sizeof(char)*(strlen(fileName)+1));
                if(entFileName == NULL)
                {
                    printf("Error in line #%d: Memory allocation failed.\n",lineCounter);
                    exit(1);
                }
                strcpy(entFileName,fileName);
                for(i = 0; entFileName[i-1] != '.';i++);
                entFileName[i] = 'e';
                entFileName[i+1] = 'n';
                entFileName[i+2] = 't';
                entFileName[i+3] = '\0';
                fp = fopen(entFileName,"w");
                if(fp == NULL)
                {
                    printf("Error in line #%d: Memory allocation failed.\n",lineCounter);
                    exit(1);
                }
                fprintf(fp,"%s, %d,%d\n",current->data->symbol,current->data->baseAddress,current->data->offset);
                isEntExist = true;
            }
            else
            {
                fprintf(fp,"%s, %d,%d\n",current->data->symbol,current->data->baseAddress,current->data->offset);
            }
        }
        current = current->next;
    }
    if(isEntExist == true)
    {
        fclose(fp);
        free(entFileName);
    }
}

void createExtFile(char* fileName,bool open)
{
    int i;
    char* extFileName = NULL;
    if(open == true)
    {
        extFileName = (char *) malloc(sizeof(char) * (strlen(fileName) + 1));
        if (extFileName == NULL)
        {
            printf("Error in line #%d: Memory allocation failed.\n", lineCounter);
            exit(1);
        }
        strcpy(extFileName, fileName);
        for (i = 0; extFileName[i - 1] != '.'; i++);
        extFileName[i] = 'e';
        extFileName[i + 1] = 'x';
        extFileName[i + 2] = 't';
        extFileName[i + 3] = '\0';
        fpExt = fopen(extFileName, "w");
        if (fpExt == NULL)
        {
            printf("Error in line #%d: Memory allocation failed.\n", lineCounter);
            exit(1);
        }
    }
    else
    {
        fclose(fpExt);
        free(extFileName);
    }
}

void codeMissingPart(char* line)
{
    int wordsBeforeCoding = 2;
    char *token = NULL;
    char tempLine[MAX_SIZE_LEN] = {0};
    int i;
    int operandNumbers;
    strcpy(tempLine,line);
    operandNumbers = howMuchOperands(line);
    if(operandNumbers == 0)
    {
        secondPassIC += instructionTable[secondPassIC - IC_SET].L;
        return;
    }
    token = strtok(tempLine,WHITE_CHARS);
    for(i = 0;i < operandNumbers;i++)
    {
        token = strtok(NULL," \t\r\v\n\f,");
        if(isRegister(token))
        {

        }
        else if(token[0] == '#')
        {
            wordsBeforeCoding++;
        }
        else
        {
            if(operandNumbers == 1)
            {
                codeLabel(token, wordsBeforeCoding, i + 2);
            }
            else
            {
                codeLabel(token, wordsBeforeCoding, i + 1);
            }
        }
    }
    secondPassIC += instructionTable[secondPassIC - IC_SET].L;
}

int howMuchOperands (char *line)
{
    int opcode = 0;
    opcode = whatAction(line);
    if(opcode >= 14)
        return 0;
    if(opcode >= 5)
        return 1;
    if(opcode >= 0)
        return 2;
    return -1;
}

void codeLabel(char* label,int WordsBeforeCoding,int operandNumber)
{
    int registerIndex = 0;
    registerIndex = strcspn(label,"[");
    if(registerIndex == strlen(label))
    {
        directAddressing(label,WordsBeforeCoding);
    }
    else
    {
        indexAddressing(label,WordsBeforeCoding,operandNumber);
    }
}

void indexAddressing(char* label,int wordsBeforeCoding,int operandNumber)
{
    char labelName[MAX_SIZE_LABEL] = {0};
    int registerNumber = 0;
    if(!isOperandLegal(label))
    {
        printf("Error in line #%d: The operand %s is illegal\n",lineCounter,label);
        isError = true;
        return;
    }
    registerNumber = getRegisterNumber(label);
    if(registerNumber < 10 || registerNumber > 15)
    {
        printf("Error in line #%d: The operand %s is illegal\n",lineCounter,label);
        isError = true;
        return;
    }
    if(operandNumber == 1)
        instructionTable[(secondPassIC+1)-IC_SET].W.w2.sourceRegister = registerNumber;
    else if(operandNumber == 2)
        instructionTable[(secondPassIC+1)-IC_SET].W.w2.targetRegister = registerNumber;
    getLabelName(label,labelName);
    directAddressing(labelName,wordsBeforeCoding);
}

void getLabelName(char* label,char* labelName)
{
    int i = 0;
    for(i = 0; label[i] != '['; i++)
    {
        labelName[i] = label[i];
    }
}

int getRegisterNumber(char* label)
{

    char registerNumberInString[2] = {0};
    int i;
    for(i = 0 ; label[i-2] != '[' ; i++);
    registerNumberInString[0] = label[i];
    i++;
    if(isdigit(label[i]) != 0)
        registerNumberInString[1] = label[i];
    return atoi(registerNumberInString);
}

bool isOperandLegal(char* label)
{
    int i;
    for(i = 0 ; label[i] != '[' ; i++);
    i++;
    if(label[i] != 'r')
        return false;
    i++;
    if(isdigit(label[i]) == 0)
        return false;
    i++;
    if(label[i] != ']' || label[i+1] != '\0')
    {
        if(isdigit(label[i]) == 0 || label[i+1] != ']' || label[i+2] != '\0')
        {
            return false;
        }
    }
    return true;
}

void directAddressing(char* label,int wordsBeforeCoding)
{
    bool isExtern = false;
    int offset = 0;
    int baseAddress = 0;
    if(!getAddressAndOffset(label,&offset,&baseAddress,&isExtern))
    {
        printf("Error in line #%d: the label \"%s\" does not exist.\n",lineCounter,label);
        return;
    }
    setLabelWords(label,baseAddress,offset,isExtern,wordsBeforeCoding,secondPassIC);
}

bool getAddressAndOffset(char* label,int* offset,int* baseAddress,bool* isExtern)
{
    Node* current = headLabels;
    while(current != NULL)
    {
        if(strcmp(current->data->symbol,label) == 0)
        {
            *offset = current->data->offset;
            *baseAddress = current->data->baseAddress;
            *isExtern = current->data->attributes[3];
            return true;
        }
        current = current->next;
    }
    return false;
}

