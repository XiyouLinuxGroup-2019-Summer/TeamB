#include <stdlib.h> 
#include <stdio.h> 
#include <readline/readline.h> 
int main(void) 
{
    char *str=readline("my_Shell$$:");
    free(str);
    return 0;
}


