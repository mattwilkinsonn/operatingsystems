#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Error: Too few arguments");
        return;
    }
    // printf("%s", *argv);

    char *currentArg;
    int total = 1;
    int num;

    for (int i = 1; i < argc; i++)
    {
        currentArg = argv[i];
        num = (int)strtol(currentArg, (char **)NULL, 10);
        total *= num;
    }

    printf("%d \n", total);
}