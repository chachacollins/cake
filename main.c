#include <stdio.h>
#include "utils.h"

int main(void)
{
    const char* source = readFile("CAKEFILE");
    printf("%s\n", source);
}
