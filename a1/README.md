# Assignment 1 for Operating Systems Class

## Matt Wilkinson, Spring 2021, Prof Japkowicz, American University

Calculator shell using processes and pipes, as well as the system calls `fork()`. `wait()`, `execvp()`, `exit()`.

I used the regex library in C to parse the arguments passed to the shell as well.

The compile.sh script will use gcc to compile all of the files with the correct file names. This is necessary for the program to function as if the files are named differently they cannot be opened by `execvp()`.

To run the program, first run the compile.sh script with `./compile.sh`, then start the `calculator` program using the command: `./calculator`
