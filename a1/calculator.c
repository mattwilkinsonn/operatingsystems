#include <stdio.h>
#include <sys/types.h>
#include <stdbool.h>

int parse(char *line, char **argv)
{
    int numArgs = 0;
    while (*line != '\0')
    { /* if not the end of line ....... */
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0'; /* replace white spaces with 0    */
        *argv++ = line;     /* save the argument position     */
        numArgs++;
        while (*line != '\0' && *line != ' ' &&
               *line != '\t' && *line != '\n')
            line++; /* skip the argument until ...    */
    }
    *argv = '\0'; /* mark the end of argument list  */
    return numArgs;
}

void execute(char **argv, int numArgs)
{
    pid_t pid;
    int status;

    if ((pid = fork()) < 0)
    { /* fork a child process           */
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0)
    { /* for the child process:         */
        switch (*argv[0])
        {
        case 'A':
            execvp("./add", argv);
            break;
        case 'S':
            execvp("./subtract", argv);
            break;
        case 'M':
            execvp("./multiply", argv);
            break;
        case 'D':
            execvp("./divide", argv);
            break;
        default:
            printf("Invalid input \n");
            break;
        }

        // if (execvp(*argv, argv) < 0)
        // { /* execute the command  */
        //     printf("*** ERROR: exec failed\n");
        //     exit(1);
        // }
    }
    else
    {                                /* for the parent:      */
        while (wait(&status) != pid) /* wait for completion  */
            ;
    }
}

void main(void)
{
    char line[1024]; /* the input line                 */
    char *argv[64];  /* the command line argument      */
    char operation;
    int numArgs;

    while (1)
    {                          /* repeat until done ....         */
        printf("Calculator>"); /*   display a prompt             */
        gets(line);            /*   read in the command line     */
        // printf("\n");
        numArgs = parse(line, argv); /*   parse the line               */
        operation = *argv[0];
        if (strcmp(argv[0], "exit") == 0)
        {
            exit(0);
        }
        // char firstNum = *argv[1];
        execute(argv, numArgs); /* otherwise, execute the command */
    }
}

void add(char **argv, int numArgs)
{
    if (numArgs < 2)
    {
        printf("Error: Too few arguments");
        return;
    }

    char *currentArg = argv[1];
    int total = 0;
    int num;

    for (int i = 0; i < numArgs - 1; i++)
    {
        num = *currentArg - '0';
        total += num;
        currentArg += 2;
    }

    printf("%d \n", total);
}

void subtract(char **argv, int numArgs)
{
    if (numArgs > 3)
    {
        printf("Error: Too many arguments");
        return;
    }
    else if (numArgs < 3)
    {
        printf("Error: Too few arguments");
        return;
    }

    int num1 = *argv[1] - '0';
    int num2 = *argv[2] - '0';

    printf("%d \n", num1 - num2);
}

void multiply(char **argv, int numArgs)
{
    if (numArgs < 2)
    {
        printf("Error: Too few arguments");
        return;
    }

    char *currentArg = argv[1];
    int total = 1;
    int num;

    for (int i = 0; i < numArgs - 1; i++)
    {
        num = *currentArg - '0';
        total *= num;
        currentArg += 2;
    }

    printf("%d \n", total);
}

void divide(char **argv, int numArgs)
{
    if (numArgs > 3)
    {
        printf("Error: Too many arguments");
        return;
    }
    else if (numArgs < 3)
    {
        printf("Error: Too few arguments");
        return;
    }

    int num1 = *argv[1] - '0';
    int num2 = *argv[2] - '0';
    float div = num1 / num2;
    printf("%f", div);
}