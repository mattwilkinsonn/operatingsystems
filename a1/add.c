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
            execvp("./subtractnomax", argv);
            break;
        case 'M':
            execvp("./multiply", argv);
            break;
        case 'D':
            execvp("./dividenomax", argv);
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

    if (argc < 3)
    {
        printf("Error: Too few arguments\n");
        return 0;
    }
    // printf("%s", *argv);

    char *currentArg;
    int total = 0;
    int num;
    regex_t regex;
    int regex_return;
    regcomp(&regex, "^[AMSD]$", 0);

    regex_t error;
    regcomp(&error, "Error", 0);
    int error_return;

    int i = 1;
    while (i < argc)
    {
        currentArg = argv[i];
        regex_return = regexec(&regex, currentArg, 0, NULL, 0);
        if (regex_return == 0)
        {
            // printf("reg \n");
            char *returnedVal = executeSubOperation(&argv[i]);

            error_return = regexec(&error, returnedVal, 0, NULL, 0);
            if (error_return == 0)
            {
                printf("%s \n", returnedVal);
                return 0;
            }

            // printf("%s \n", returnedVal);
            num = (int)strtol(returnedVal, (char **)NULL, 10);
            total += num;
            switch (*currentArg)
            {
            case 'A':
            case 'M':
                i = argc;
                break;
            case 'S':
            case 'D':
                i += 4;
                break;
            default:
                break;
            }
        }
        else
        {
            num = (int)strtol(currentArg, (char **)NULL, 10);
            total += num;
            i++;
        }
    }

    printf("%d \n", total);
    return 0;
}