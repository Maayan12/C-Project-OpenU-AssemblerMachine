#include "structure.h"

/*firstPass ---------------------------------------*/

/*
 *This method receives a string that represents a files path and codes the first path-
 * the first faze of coding the file in the assembler.
 * input: char*
 * output: nothing
 */
void readSourceFile(char *arr);

/*
 * This method receives a string that represents a code line and sends the line to other functions
 * to identify the line and to code it.
 * input: char [] (a string)
 * output: nothing
 */
void codeLine(char *line);

/*
 * This function receives a string that represents a code line and checks whether
 * the code line is an instruction line.
 * if the code line is an instruction line the function continues to code the line
 * input: char [] (a string)
 * output: true if the line is an instruction line and false if not.
 */
bool codeInstruction(char line[MAX_SIZE_LEN]);

/*
 * This function receives a string that represents a number in immediate addressing mode and returns
 * it as the operand (if the number is negative it calculates it using the Two's complement)
 * input: char* (string)
 * output: an int-the operand
 */
int getImmediateAddressOperand(char* token);

/*
 *This function receives a string and its length and returns the string as an int if it is a number
 * if not it returns INT_MAX
 * input: char[],an int
 * output: an int- the number
 */
int getNum(char arr [],int size);

/*
 * This function receives a string and returns whether the string is a register or not.
 * input: char* (=char arr = string)
 * output: true if the string is a register and false if not.
 */
bool isRegister(char* isRegister);

/*
 *This function receives a condition a pointer to the addressing mode operand,
 * a pointer to the source/target operand, the index to the actions table and a part of a line that is
 * supposed to be the operands.
 * the function returns whether the operand is in immediate addressing mode. if it is,
 * but the funct isn't supposed to allow that(we know using the condition), we print error else
 * (it is in immediate addressing mode and the funct is supposed to allow that)
 * the function sets the addressing mode operand to 0 and calculates the operand
 * input: a bool, an int*, an int*,a char*, an int
 * output: true if the operand is in immediate addressing mode and false if not.
 *
 */
bool immediateAddress(bool opcodeCondition, int* address, int* operand,char* token);

/*
 * This method receives a string and checks whether the string is a valid name of a label.
 * if not it updates a flag that represents error to be true.
 * input: char [] (a string)
 * output: nothing
 */
void checkLabelsName(char name[MAX_SIZE_LABEL]);

/*
 * This function receives a string that represents a code line and checks whether
 * the code line defines a label (using :).
 * if the code line defines a label we add it to the list of labels and code the line
 * input: char [] (a string)
 * output: true if the line contains a defined label and false if not.
 */
bool ifDefiningLabel(char line[MAX_SIZE_LEN]);

/*
 * This function receives a string that represents a code line and checks whether
 * the code line defines an extern label (using .extern).
 * if the code line defines an extern label we add it to the list of labels and code the line
 * input: char [] (a string)
 * output: true if the line contains an extern label and false if not.
 */
bool ifExtern(char line[MAX_SIZE_LEN]);

/*
 * This function receives a string that represents a code line and checks whether
 * the code line defines an entry label (using .entry).
 * input: char [] (a string)
 * output: true if the line contains an entry label and false if not.
 */
bool ifEntry(char line[MAX_SIZE_LEN]);

/*
 * This function receives a string that represents a code line and checks whether
 * the code line is a data line.
 * if the code line is a data line the function continues to code the line
 * input: char [] (a string)
 * output: true if the line is a data line and false if not.
 */
bool codeData(char line[MAX_SIZE_LEN]);

/*
 * This function receives a char arr (string) that represents a function and returns the index of
 * the function in the actions table-
 * (an array of structs created in order to save all of the functions and the opcode and the funct)
 * if the string isn't an actual function -1 is returned.
 * input: char [] (string)
 * output: int- the index
 */
int getActionIndex(char func[SIZE_FUNCT_NAME]);

/*
 * This method receives an int that represents an index in the actions table and a char pointer
 * that represents an instruction type code line, and codes the words.
 * (by sending arguments to coding functions)
 * input: an int, a string (char*)
 * output: nothing
 */
void setWords(int index, char *token);


/*
 *This function receives a condition a pointer to the addressing mode operand,
 * a pointer to the source/target operand, the index to the actions table and a part of a line that is
 * supposed to be the operands.
 * the function returns whether the operand is in register Direct addressing mode. if it is,
 * but the funct isn't supposed to allow that(we know using the condition), we print error else
 * (it is in register Direct addressing mode and the funct is supposed to allow that)
 * the function sets the addressing mode operand to 3 and calculates the operand
 * input: a bool, an int*, an int*,a char*, an int
 * output: true if the operand is in register Direct addressing mode and false if not.
 *
 */
bool registerDirect(bool opcodeCondition, int* address, int* operand,char* token);

/*
 *This function receives a condition a pointer to the addressing mode operand,
 * a pointer to the source/target operand, the index to the actions table and a part of a line that is
 * supposed to be the operands.
 * the function returns whether the operand is in addressing Index addressing mode. if it is,
 * but the funct isn't supposed to allow that(we know using the condition), we print error else
 * (it is in addressing Index addressing mode and the funct is supposed to allow that)
 * the function sets the addressing mode operand to 2 and calculates the operand
 * input: a bool, an int*, an int*,a char*, an int
 * output: true if the operand is in addressing Index addressing mode and false if not.
 */
bool addressingIndex(bool opcodeCondition, int* address, int* operand,char* token);

/*
 *This function receives a condition a pointer to the addressing mode operand,
 * a pointer to the source/target operand, and three booleans that represents
 * whether the operand is in other addressing modes.
 * the function returns whether the operand is in direct addressing mode. if it is,
 * but the funct isn't supposed to allow that(we know using the condition), we print error else
 * (it is in direct addressing mode and the funct is supposed to allow that)
 * the function sets the addressing mode operand to 1 and calculates the operand
 * input: a bool, an int*, an int*,a char*, an int
 * output: true if the operand is in direct addressing mode and false if not.
 */
void isDirect(bool opcodeCondition, bool isImmediate, bool isRegisterDirect, bool isAddressingIndex, int* address, int* operand);


/* preAssembler ----------------------------------------*/

/*
 * This method gets the name of a '.as' file and replaces all the macros defined in the code.
 * It prints the new code to the '.am' file it creates.
 * input: char*.
 * output: nothing.
 */
void preAssemblerReadSourceFile(char* fileName);
/*
 * If there is a label the function returns the position index after the label If there is no function label returns 0.
 * input: char[] (the current line).
 * output: int (the index after the label if the label exist.
 */
int skipLabel(char* line);
/*
 * This function checks whether the current line should be skipped.
 * input: char [] (the current line).
 * output: true if this line should be skipped and false if not.
 */
bool isSkipableLine(char *line);
/*
 * This method adds the current line to be the contents of the last defined macro.
 * It allocates more space in memory and puts the current line into it.
 * input: char[] (the current line).
 * output: nothing.
 */
void addContent(char* line);
/*
 * This method deletes the linked list of macros.
 * input: nothing.
 * output: nothing.
 */
void deleteLinkedList();
/*
 * This function checks if there is a macro in the macro name that is now define.
 * input: nothing (the name is global variable).
 * output: true if the macro with the same name exist and false if not.
 */
bool doesMacroExists();
/*
 * This function creates a new variable of type struct MacroData.
 * It initializes the name in the name of the currently defined macro and initializes the content to NULL.
 * input: nothing (the name is global variable).
 * output: struct MacroData (the one that the function just created).
 */
MacroData* macroDataAdd();
/*
 * This method initializes the head of the linked list.
 * It allocates a memory space to the head of the list and initializes its values to NULL.
 * input: nothing (the head of the linked list is global variable).
 * output: nothing.
 */
void createEmptyList();
/*
 * This function checks whether the line it received is a line that contains only the word "endm" except for white spaces.
 * input: char[] (the current line).
 * output: true if the line contains only the word "endm" and false if not.
 */
bool isEndm(char* line);
/*
 * This function checks whether the current line is a line with a macro definition, then it checks whether such a name already exists and whether the definition line is valid.
 * It updates the global name variable in the macro name.
 * input: char[] (the current line).
 * output: true if the current line is a line where a macro is defined but such a macro already exists and false if not.
 */
bool getMacroName(char* line);
/*
 * This method adds an item to the linked macro list using the data it receives.
 * input: struct MacroData* (The data for the item added in the method).
 * output: nothing.
 */
void macroNodeAdd(MacroData *newData);
/*
 * This function replaces the last character in a line from '\n' to '\0'.
 * It checks whether the line doesn't exceed 81 characters.
 * input: char[] (the current line).
 * output: true if the current line size does not exceed the specified size and false if it is.
 */
bool changeBackSlashNToBackSlash0(char* line);
/*
 * This method goes over each word in the current line and checks if the word is the name of a defined macro.
 * If not it prints the word itself.
 * If so, it prints the macro contents to '.am' file.
 * input: char[] (the current line).
 * output: nothing.
 */
void replaceMacroNameWithMacroContent(char* line);

/* secondPass ---------------------------------------*/

/*
 * This method gets a '.am' file And encodes everything that could not be encoded in the first transition.
 * It creates a '.ob' file to which it prints the memory image of the code.
 * input: char*
 * output: nothing.
 */
void secondPassReadSourceFile(char *arr);
/*
 * This function checks whether the current line is a line that has an entry definition.
 * input: char[] (the current line).
 * output: true if it is a line that has an entry and false if not.
 */
bool isEntry(char* line);
/*
 * This method is called after knowing that the current line is a line with an entry definition.
 * The method finds the name of the label after the entry and puts its name in the variable of the name.
 * input: char[] (the current line), *char (variable to put the name in to).
 * output: nothing.
 */
void getEntryName(char* line,char* name);
/*
 * The function checks if its label made entry exists.
 * If it exists, add in the table of symbols that the label is also an entry.
 * input: char[] (the name of the label).
 * output: true if the label is exists and false if not.
 */
bool addEntry(char* name);
/*
 * The function is called when you know that the current line is a instruction line.
 * It checks the instruction in a line using another function and checks the number of operands in the instruction.
 * input: char[] (the current line).
 * output: the number of the instruction word operator and if there is no instruction word it return -1;
 */
int howMuchOperands (char *line);
/*
 * This method codes the label into the appropriate place in the instruction table.
 * It finds the addressing method appropriate for the label (directAddressing or indexAddressing).
 * input: char[] (the current line),int,int (both to pass to the addressing method).
 * output: nothing.
 */
void codeLabel(char* label,int WordsBeforeCoding,int operandNumber);
/*
 * This method checks with the help of a function what the offset and base address of the label whose name it was given and whether the label is defined as extern.
 * If there is no label of the same name she was given she stops working.
 * It sends this data and the number of words that need to be jumped to another function which adds them to the memory image.
 * input: char[] (the label), int (the number of words that you need to jump).
 * output: nothing.
 */
void directAddressing(char* label,int wordsBeforeCoding);
/*
 * The function searches for a defined label with the same name it was given.
 * If such a label exists it puts the value of the offset, of the base address and the answer of the test whether the label is defined as extern into the appropriate pointers.
 * input: char[] (the label name to check),int* (where it puts the value of the offset), int* (Where it puts the value of the base address), bool* (Where it puts the value of the extern).
 * output: true if there is a label defined as the name it was given and false if not.
 */
bool getAddressAndOffset(char* label,int* offset,int* baseAddress,bool* isExtern);
/*
 * The function checks whether the name of the label is a valid name
 * input: char[] (the label name)
 * output: true if this is a valid name and false if not.
 */
bool isOperandLegal(char* label);
/*
 * This function receives an operand of a label and a register and returns the register number.
 * input: char[] (operand of label and register).
 * output: int (register number).
 */
int getRegisterNumber(char* label);
/*
 * This method is given the name of an operand of a label and a register and separates the name of the label into another variable.
 * input: char[] (the operand name), char[] (the variable to put the name into).
 * output: nothing.
 */
void getLabelName(char* label,char* labelName);
/*
 *This method is given the name of an operand which is both a label and a hamster.
 * It encodes the hamster using data it receives.
 * It sends the label alone for encoding in another function.
 * input: char* (the operand name), int (to pass the other function), int (data to code the register).
 * output: nothing.
 */
void indexAddressing(char* label,int wordsBeforeCoding,int operandNumber);
/*
 * This method accepts the current line and encodes the things that could not be encoded in the first pass with the help of other functions.
 * input: char[] (the current line).
 * output: nothing.
 */
void codeMissingPart(char* line);
/*
 * This function checks whether the current line is a line that can be skipped.
 * input: char[] (the current line).
 * output: true if this is a line that can be skipped and false if not.
 */
bool isSkip(char* line);
/*
 * This function creates a '.ob' file and prints the memory image to it.
 * input: char[] (the file name to create the new file).
 * output: nothing.
 */
void createObFile(char* fileName);
/*
 * This function creates an '.ent' file only if there is an entry define.
 * It prints the base address and offset of all the labels that defined them as an 'entry' to the file.
 * input: char[] (the file name to create the new file).
 * output: nothing.
 */
void createEntFile(char* arr);
/*
 * This function creates an '.ext' file only if there is an extern define.
 * It prints the IC of words used in the base address and offsets all the labels that defined them as 'extern' to the file.
 * input: char[] (the file name to create the new file). bool (to open or close the file).
 * output: nothing.
 */
void createExtFile(char* fileName,bool open);
/*
 * This function check is there is any 'extern' definition in the code.
 * input: nothing
 * output: True if there is 'extern' and false if not.
 */
bool isExtExist();

/*tables ----------------------------------------------*/
/*
 * This method updates the data labels at the end of the first pass to prepare to the second pass.
 * input: nothing
 * output: nothing
 */
void updateLabels();
/*
 * This method receives a details of a symbol: a name of the symbol, the type (data/code),
 * the value in the memory (DC/IC), the number of the line, and a pointer to boolean that contains if there
 * are errors.
 * The method adds the symbol to the list of symbols by using the details.
 * input: char*, int,int, int, bool*
 * output: nothing
 */
void add(char* symbol, int type,int value, int lineCounter, bool* isError);
/*
 * This method free's the memory of the dinamicly allocated variables int the noted list of labels
 * (it free's the members of the list too- the labels)
 * input: nothing
 * output: nothing
 */
void freeList();
/*
 * this method reset the data words array and the instructions words array.
 * input: nothing.
 * output: nothing.
 */
void resetMemmory();

/*
 * This function receives an int that represents an address in the fake memory and returns its
 * base address-the first number that is smaller than the received value that can be divided by 16.
 * (mod 16).
 * input: an int
 * output: an int- the address
 */
int getBaseAddress(int value);

/*words ---------------------------------------------------*/

/*
 * This method sets the first type of word in an instruction sentence using an int that represents
 * the index in an array of structs that details all of the functions and their opcode and funct.
 * input: an int
 * output: nothing
 */
void setFirstWord(int actionIndex);

/*
 * This method sets the second type of word in an instruction sentence using received parameters
 * that represent the needed information in order to set the word:
 * source addressing mode,target addressing mode,source operand, target operand and the funct.
 * input: an int,an int,an int,an int,an int
 * output: nothing
 */
void setSecondWord(int targetAddress, int targetRegister,int sourceAddress,int sourceRegister,int funct);

/*
 * This method sets the third type of word in an instruction or data sentence using received parameters
 * that represent the needed information in order to set the word:
 * the operand to code the word, a boolean variable that states whether the sentence is of type data or
 * instruction.
 * input: an int,a boolean
 * output: nothing
 */
void setThirdWord(int operand, bool isData);
/*
 * This method sets the 2 label words in an instruction or data sentence using received parameters
 * that represent the needed information in order to set the word:
 * The offset and the base address of the label and the boolean variable that tell if the label is extern.
 * intput: char[] (label name),int (baseaddress), int (offset), bool (is the label extern), int (word that need to skip), int (current IC).
 * output: nothing.
 */
void setLabelWords(char* label,int baseAddress,int offset,bool isExtern,int wordsBeforeCoding,int secondPassIC);

