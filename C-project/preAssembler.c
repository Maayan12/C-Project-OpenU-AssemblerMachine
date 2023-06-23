#include "assembler.h"

FILE *fpWrite;
MacroNode *head;
char name[MAX_SIZE_LEN] = {0};/*name of macro macro*/
int lineCounter;

void preAssemblerReadSourceFile(char *fileName)
{
    bool inMacro = false;
    FILE *fpRead = NULL;
    char *asFileName = NULL;
    char *amFileName = NULL;
    char line[MAX_SIZE_LEN] = {0};
    fpWrite = NULL;
    head = NULL;
    memset(name, 0, sizeof(name));
    lineCounter = 0;
    createEmptyList();
    asFileName = (char*)malloc(sizeof(char)*(strlen(fileName)+4));
    if(asFileName == NULL)
    {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }
    amFileName = (char*)malloc(sizeof(char)*(strlen(fileName)+4));
    if(amFileName == NULL)
    {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }
    strcpy(asFileName,fileName);
    strcpy(amFileName,fileName);
    sprintf(asFileName,"%s.as",fileName);
    sprintf(amFileName,"%s.am",fileName);
    fpRead = fopen(asFileName,"r");/*open "filename".as*/
    if(fpRead == NULL)
    {
        printf("Error: could not open the file \"%s\".\n",asFileName);
        return;
    }
    fpWrite = fopen(amFileName,"w");/*open "filename".am*/
    if(fpWrite == NULL)
    {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }
    while(fgets(line, sizeof(line), fpRead) != NULL)
    {
        lineCounter++;
        memset(name, 0, sizeof(name));
        if(!changeBackSlashNToBackSlash0(line))
        {
            printf("Error in line #%d: the line bigger the 81 chars.\n",lineCounter);
            return;
        }
        if(isSkipableLine(line) == false)
        {
            if(inMacro == false)
            {
                if(getMacroName(line))
                {
                    return;
                }
                if(name[0] != 0)
                {
                    macroNodeAdd(macroDataAdd());
                    inMacro = true;
                }
                else
                {
                    replaceMacroNameWithMacroContent(line);
                }
            }
            else
            {
                if(!isEndm(line))
                {
                    addContent(line);
                }
                else
                {
                    inMacro = false;
                }
            }
        }
    }
    fclose(fpRead);
    fclose(fpWrite);
    readSourceFile(amFileName);
    free(amFileName);
    free(asFileName);
    deleteLinkedList();
}

bool isSkipableLine(char* line)
{
    int i;
    char tempLine[MAX_SIZE_LEN] = {0};
    char *token = NULL;
    strcpy(tempLine,line);
    for(i = 0; isspace(tempLine[i]) != 0; i++);
    if(tempLine[i+1] == '\0')
    {
        return true;
    }
    token = strtok(tempLine,WHITE_CHARS);
    if(token[0] == ';')
        return true;
    return false;
}

bool changeBackSlashNToBackSlash0(char* line)
{
    unsigned int lastIndex = strcspn(line, "\n");
    if(lastIndex == MAX_SIZE_LEN-1)/*to check if the line is bigger than 81*/
    {
        return false;
    }
    line[lastIndex] = '\0';
    return true;
}

void replaceMacroNameWithMacroContent(char* line)
{
    bool replaced = false;
    MacroNode* current = head;
    char* token = NULL;
    char tempLine[MAX_SIZE_LEN] = {0};
    strcpy(tempLine,line);
    token = strtok(tempLine,WHITE_CHARS);
    if(token == NULL)/*epmty line*/
    {
        fprintf(fpWrite,"\n");
        return;
    }
    while(token != NULL)
    {
        current = head;
        replaced = false;
        while(current->next != NULL)
        {
            if(strcmp(current->data->name,token) == 0)
            {
                fprintf(fpWrite,"%s",current->data->content);
                replaced = true;
            }
            current = current->next;
        }
        if(replaced == false)
        {
            fprintf(fpWrite,"%s ",token);
        }
        token = strtok(NULL,WHITE_CHARS);
    }
    if(replaced == false)
    {
        fprintf(fpWrite,"\n");
    }
}

void createEmptyList()
{
    head = (MacroNode*) malloc(sizeof(MacroNode));
    if(head == false)
    {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }
    head->data = NULL;
    head->next = NULL;
}

bool getMacroName(char *line)
{
    char tempLine[MAX_SIZE_LEN] = {0};
    char *strp = NULL;
    char *token = NULL;
    strcpy(tempLine, line);
    strp = tempLine;
    strp += skipLabel(strp);
    token = strtok(strp, WHITE_CHARS);
    if(token == NULL)
    {
        return false;
    }
    if(strcmp(token, "macro") != 0)
    {
        return false;
    }
    token = strtok(NULL,WHITE_CHARS);

    if(token == NULL)/*macro without name*/
    {
        return false;
    }
    strcpy(name,token);
    token = strtok(NULL, WHITE_CHARS);

    if(token != NULL)/*illegal macro name*/
    {
        memset(name, 0, sizeof(name));
        return false;
    }
    if(doesMacroExists())
    {
        printf("Error in line #%d: macro %s allready exists.\n",lineCounter,name);
        memset(name, 0, sizeof(name));
        return true;
    }
    return false;
}

bool doesMacroExists()
{
    MacroNode* current = head;
    while(current->next != NULL)
    {
        if(strcmp(current->data->name,name) == 0)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

int skipLabel(char *line)
{
    int i;
    for(i = 0; line[i]; i++)
    {
        if(line[i] == ':')
            return i +1;
    }
    return 0;
}

void macroNodeAdd(MacroData *newData)
{
    /* allocate node */
    MacroNode *newNode = (MacroNode*) malloc(sizeof(MacroNode));
    if (newNode == false) {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }

    /* put in the data */
    newNode->data = (MacroData*) malloc(sizeof(MacroData));
    if (newNode->data == false) {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }
    newNode->data = newData;

    /* Make next of new node as head */
    newNode->next = head;

    /* move the head to point to the new node */
    head = newNode;
}

MacroData* macroDataAdd()
{
    MacroData* newMacroData;
    newMacroData =(MacroData*) malloc(sizeof(MacroData));
    if(newMacroData == false)
    {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }
    newMacroData->name =(char*) malloc(sizeof(char) * strlen(name));
    if(newMacroData->name == false)
    {
        printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
        exit(1);
    }
    strcpy(newMacroData->name,name);
    newMacroData->content = NULL;
    return newMacroData;
}

void addContent(char* line)
{
    static int size = 0;
    MacroNode* current = head;
    size += (int)(strlen(line)+1);
    if(current->data->content == NULL)
    {
        current->data->content = (char*) malloc(sizeof(char) * size);
        if(current->data->content == NULL)
        {
            printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
            exit(1);
        }
        strcpy(current->data->content,"");
    }
    else
    {
        current->data->content = (char *) realloc(current->data->content,sizeof(char) * size);
        if(current->data->content == NULL)
        {
            printf("Error in line #%d: Memmory allocation failed.\n",lineCounter);
            exit(1);
        }
    }
    strcat(current->data->content,line);
    strcat(current->data->content,"\n");
}

bool isEndm(char* line)
{
    char *token = NULL;
    char tempLine[MAX_SIZE_LEN] = {0};
    strcpy(tempLine,line);
    token = strtok(tempLine, WHITE_CHARS);
    if(token == NULL)
    {
        return false;
    }
    if(strcmp(token,"endm") != 0)
    {
        return false;
    }
    token = strtok(NULL, WHITE_CHARS);
    if(token == NULL)
    {
        return true;
    }
    return false;
}

void deleteLinkedList()
{
    MacroNode* current = head;
    MacroNode* next = NULL;
    while(current->next != NULL)
    {
        next = current->next;
        free(current->data->name);
        free(current->data->content);
        free(current->data);
        free(current);
        current = next;
    }
    head = NULL;
}

