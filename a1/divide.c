#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(int argc, char *argv[])
{
    // printf("%d", argc);
    if (argc > 3)
    {
        printf("Error: Too many arguments \n");
        return;
    }
    else if (argc < 3)
    {
        printf("Error: Too few arguments \n");
        return;
    }
    int num1 = (int)strtol(argv[1], (char **)NULL, 10);
    int num2 = (int)strtol(argv[2], (char **)NULL, 10);

    printf("%d \n", num1 / num2);
}