#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

char *executeSubOperation(char *argv[])
{
    int fd[2];
    pid_t pid;
    int status;

    char *currentArg;
    currentArg = argv[0];
    // printf("arg: %s \n", currentArg);
    char *smallArgv[3];

    if (pipe(fd) == -1)
    {
        fprintf(stderr, "Pipe Failed \n");
        exit(1);
    }

    if ((pid = fork()) < 0)
    { /* fork a child process           */
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    }
    else if (pid == 0)
    { /* for the child process:         */
        // printf("arg1: %s \n", argv[1]);
        close(fd[0]);   // close read end
        dup2(fd[1], 1); // send stdout to fd
        dup2(fd[1], 2); // send sterr to fd
        close(fd[1]);
        switch (*currentArg)
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
    }
    else
    {
        close(fd[1]);                /* for the parent:      */
        while (wait(&status) != pid) /* wait for completion  */
            ;
        char buffer[1024];
        char *p = buffer;
        int ret = read(fd[0], buffer, sizeof(buffer));
        // printf("ret: %d \n", ret);
        // printf("err: %s \n", strerror(errno));
        // printf("buffer: %s \n", buffer);
        return p;

        // read output from fd here and then return it as char
    }
}

int main(int argc, char *argv[])
{

    // printf("argc: %d \n", argc);
    // printf("argv[0]: %s \n", argv[0]);

    // printf("argv[1]: %s \n", argv[1]);
    // printf("argv[2]: %s \n", argv[2]);

    if (argc < 4)
    {
        printf("Error: Too few arguments \n");
        return 0;
    }

    int i = 1;
    char *currentArg;
    regex_t regex;
    int regex_return;
    regcomp(&regex, "^[AMSD]$", 0);
    int inputIdx = 0;
    int resolvedInputs[2];

    regex_t error;
    regcomp(&error, "Error", 0);
    int error_return;

    while (i < argc - 1)
    {
        if (inputIdx > 1)
        {
            printf("Error: Too many arguments \n");
            return 0;
        }

        currentArg = argv[i];

        regex_return = regexec(&regex, currentArg, 0, NULL, 0);
        if (regex_return == 0)
        {
            char *returnedVal = executeSubOperation(&argv[i]);

            error_return = regexec(&error, returnedVal, 0, NULL, 0);
            if (error_return == 0)
            {
                printf("%s \n", returnedVal);
                return 0;
            }

            resolvedInputs[inputIdx] = (int)strtol(returnedVal, (char **)NULL, 10);
            switch (*currentArg)
            {
            case 'A':
            case 'M':
                i = argc; // ends while loop
                break;
            case 'S':
            case 'D':
                i += 4;
                break;
            default:
                break;
            }
            inputIdx++;
        }
        else
        {
            resolvedInputs[inputIdx] = (int)strtol(currentArg, (char **)NULL, 10);
            i++;
            inputIdx++;
        }
    }

    int subtracted = resolvedInputs[0] - resolvedInputs[1];

    printf("%d \n", subtracted);
    return 0;
}