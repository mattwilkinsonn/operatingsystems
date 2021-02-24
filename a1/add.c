#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

char *executeSubOperation(char *argv[])
{
    int fd[2]; // pipe to communicate to parent
    pid_t pid;
    int status;

    char *currentArg;
    currentArg = argv[0];
    // printf("arg: %s \n", currentArg);

    // creates pipe
    if (pipe(fd) == -1)
    {
        fprintf(stderr, "Pipe Failed \n");
        exit(1);
    }

    //creates fork
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
        close(fd[1]);   // close write end after redirecting sdout and sterr
        switch (*currentArg)
        {
        case 'A':
            execvp("./add", argv);
            break;
        case 'S':
            execvp("./subtractnomax", argv); // uses nomax versions as these operations should not check for too many arguments, as the parent add/multiply process can take the remaining arguments.
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
        int ret = read(fd[0], buffer, sizeof(buffer)); // reads input into fd (that was outputted to stdout and then redirected to fd)
        // printf("ret: %d \n", ret);
        // printf("err: %s \n", strerror(errno));
        // printf("buffer: %s \n", buffer);
        return p; // returns pointer to that read input.
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
    regcomp(&regex, "^[AMSD]$", 0); // regex to check for an operation symbol

    regex_t error;
    regcomp(&error, "Error", 0);
    int error_return;

    int i = 1;
    while (i < argc)
    {
        currentArg = argv[i];
        regex_return = regexec(&regex, currentArg, 0, NULL, 0); // checks for op symbol
        if (regex_return == 0)
        {
            // printf("reg \n");
            char *returnedVal = executeSubOperation(&argv[i]); // forks new process, runs operation and returns its' output

            error_return = regexec(&error, returnedVal, 0, NULL, 0);
            if (error_return == 0) // checks if returned value includes Error, indiciating that it did not return correctly.
            {
                printf("%s \n", returnedVal);
                return 0;
            }

            // printf("%s \n", returnedVal);
            num = (int)strtol(returnedVal, (char **)NULL, 10); // converts string number into int
            total += num;                                      // adds it to total
            switch (*currentArg)                               // based on what operation was run, skip a certain number of arguments. For A and M, end the loop. For S and D, just jump over the next 3.
            {
            case 'A':
            case 'M':
                i = argc;
                break;
            case 'S':
            case 'D':
                i += 3;
                break;
            default:
                break;
            }
        }
        else
        {
            num = (int)strtol(currentArg, (char **)NULL, 10); // if no special operation, just convert the number to int and add it.
            total += num;
            i++;
        }
    }

    printf("%d \n", total); // prints sum.
    return 0;
}