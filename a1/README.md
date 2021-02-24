# Assignment 1 for Operating Systems Class

## Matt Wilkinson, Spring 2021, Prof Japkowicz, American University

Calculator shell using processes and pipes, as well as the system calls `fork()`. `wait()`, `execvp()`, `exit()`.

I used the regex library in C to parse the arguments passed to the shell as well.

The compile.sh script will use gcc to compile all of the files with the correct file names. This is necessary for the program to function as if the files are named differently they cannot be opened by `execvp()`.

To run the program, first run the compile.sh script with `./compile.sh`, then start the `calculator` program using the command: `./calculator`

## Description of program

There are 7 different files that make up the calculator:

### calculator.c

Runs the shell, and is the parent of all the other processes. It takes the input line from the user, parses it, validates it, and then creates a new fork which runs (with `execvp()`) the appropriate child process (add, subtract, multiply, divide), passing in the parsed input line as the arguments to the new process.

### add.c, multiply.c

Both of these processes are effectively the same except that add.c adds the numbers and multiply.c multiplies them.
They both use subtractnomax.c and dividenomax.c as the processes they run for the S and D suboperations because when subtract and divide are inside add or multiply they do not need to check if there is too many arguments. See add.c for comments on how these two work.

### subtract.c, divide.c

These two are very similar again, execept for the obvious subtraction vs division at the end.
These two use the regular subtract.c and divide.c (themselves) as the suboperations as they do need to check if they have extra arguments on the end, as subtract and divide cannot take those extra arguments.
See subtract.c for comments on how these two work.

### subtractnomax.c, dividenomax.c

the subprocess for subtract (S) and divide (D) that add and multiply use if the S or D are found inside their arguments, ex.

`A 1 2 S 4 3 15`

In this case the regular subtract.c and divide.c operations will not work as those will see the extra 15 on the end of the line and return that there are too many arguments ex.

`S 4 3 15`

But when S (or D) are inside an add or multiply operation like the one further above, we still want them to just add the next two numbers and ignore the numbers afterward

It was impossible to have the same subtract function error out if there are extra arguments in one case and not error out in another, as subtract had no way of knowing about the parent process above it. There might be a crazy way to do this that I'm not aware of, but just seperating them into different files proved much easier.

These two processes do the same thing as subtract.c and divide.c, but just don't check for extra arguments, and exit the loop once they have their two numbers to subtract or divide.
