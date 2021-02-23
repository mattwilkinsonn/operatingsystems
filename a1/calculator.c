#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int parse(char *line, char **argv)
{
    while (*line != '\0')
    { /* if not the end of line ....... */
        while (*line == ' ' || *line == '\t' || *line == '\n')
            *line++ = '\0'; /* replace white spaces with 0    */
        *argv++ = line;     /* save the argument position     */
        while (*line != '\0' && *line != ' ' &&
               *line != '\t' && *line != '\n')
            line++; /* skip the argument until ...    */
    }
    *argv = '\0'; /* mark the end of argument list  */
}

void execute(char **argv)
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
    {                                     /* repeat until done ....         */
        printf("Calculator>");            /*   display a prompt             */
        fgets(line, sizeof(line), stdin); /*   read in the command line     */
        // printf("\n");
        parse(line, argv); /*   parse the line               */
        // operation = *argv[0];
        if (strcmp(argv[0], "exit") == 0)
        {
            exit(0);
        }
        // char firstNum = *argv[1];
        execute(argv); /* otherwise, execute the command */
    }
}