#include "assembler.h"

extern int IC;
extern bool isError;
extern action actions[NUMBER_OF_ACTION_TYPES];
extern word dataTable[MAX_DATA_LEN];
extern instruction instructionTable[MAX_DATA_LEN];
extern int DC;
extern FILE* fpExt;

void setFirstWord(int actionIndex)
{
    instruction inst;
    int opcode = actions[actionIndex].opcode;
    if(isError)
    {
        return;
    }
    if(IC + DC >= MAX_DATA_LEN)
    {
        printf("The program has exceeded the memory in the device.\n");
        isError = true;
        return;
    }
    inst.W.w1.opcode= 1;
    inst.W.w1.opcode <<= opcode;
    inst.W.w1.A = true;
    inst.W.w1.R = false;
    inst.W.w1.E = false;
    inst.W.w1.nineteen = false;
    inst.L=0;
    instructionTable[IC-IC_SET] = inst;
    IC++;

}

void setSecondWord(int targetAddress, int targetRegister,int sourceAddress,int sourceRegister,int funct)
{
    instruction inst;
    if(isError)
    {
        return;
    }
    if(IC + DC >= MAX_DATA_LEN)
    {
        printf("The program has exceeded the memory in the device.\n");
        isError = true;
        return;
    }
    inst.W.w2.targetAddress=targetAddress;
    inst.W.w2.targetRegister=targetRegister;
    inst.W.w2.sourceAddress=sourceAddress;
    inst.W.w2.sourceRegister=sourceRegister;
    inst.W.w2.funct=funct;
    inst.W.w2.A=1;
    inst.W.w2.R=0;
    inst.W.w2.E=0;
    inst.W.w2.nineteen=0;
    inst.L = 0;
    instructionTable[IC-IC_SET] = inst;
    IC++;
}
void setThirdWord(int operand, bool isData)
{
    instruction inst;
    if(isError)
    {
        return;
    }
    if(IC + DC >= MAX_DATA_LEN)
    {
        printf("The program has exceeded the memory in the device.\n");
        isError = true;
        return;
    }
    if(isData) {   /* data line*/
        inst.W.val.operand = operand;
        inst.W.val.E = 0;
        inst.W.val.R = 0;
        inst.W.val.A = 1;
        inst.W.val.nineteen = 0;
        dataTable[DC] = inst.W;
        DC++;
    }
    else
    {   /*code line*/
        inst.W.val.operand=operand;
        inst.W.val.E=0;
        inst.W.val.R=0;
        inst.W.val.A=1;
        inst.W.val.nineteen=0;
        inst.L = 0;
        instructionTable[IC-IC_SET] = inst;
        IC++;
    }

}

void setLabelWords(char* label, int baseAddress,int offset,bool isExtern,int wordsBeforeCoding,int secondPassIC)
{
    if(isError)
    {
        return;
    }
    instructionTable[(secondPassIC+wordsBeforeCoding)-IC_SET].W.val.operand = baseAddress;
    instructionTable[((secondPassIC+wordsBeforeCoding)-IC_SET) +1].W.val.operand = offset;
    if (isExtern)
    {
        fprintf(fpExt," %s BASE %d\n",label,secondPassIC+wordsBeforeCoding);
        fprintf(fpExt," %s OFFSET %d\n\n",label,secondPassIC+wordsBeforeCoding+1);
        instructionTable[(secondPassIC+wordsBeforeCoding)-IC_SET].W.val.A = 0;
        instructionTable[(secondPassIC+wordsBeforeCoding)-IC_SET].W.val.R = 0;
        instructionTable[(secondPassIC+wordsBeforeCoding)-IC_SET].W.val.E = 1;
        instructionTable[((secondPassIC+wordsBeforeCoding)-IC_SET) +1].W.val.A = 0;
        instructionTable[((secondPassIC+wordsBeforeCoding)-IC_SET) +1].W.val.R = 0;
        instructionTable[((secondPassIC+wordsBeforeCoding)-IC_SET) +1].W.val.E = 1;
    } else
    {
        instructionTable[(secondPassIC+wordsBeforeCoding)-IC_SET].W.val.A = 0;
        instructionTable[(secondPassIC+wordsBeforeCoding)-IC_SET].W.val.R = 1;
        instructionTable[(secondPassIC+wordsBeforeCoding)-IC_SET].W.val.E = 0;
        instructionTable[((secondPassIC+wordsBeforeCoding)-IC_SET) +1].W.val.A = 0;
        instructionTable[((secondPassIC+wordsBeforeCoding)-IC_SET) +1].W.val.R = 1;
        instructionTable[((secondPassIC+wordsBeforeCoding)-IC_SET) +1].W.val.E = 0;
    }
}

