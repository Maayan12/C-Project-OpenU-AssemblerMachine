#include "assembler.h"

int main(int argc,char *argv[])
{
    int i = 1;
    for(;i < argc; i++)
    {
        preAssemblerReadSourceFile(argv[i]);
    }
    return 0;
}

