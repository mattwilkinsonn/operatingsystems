#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

// packs all the data that is needed by the thread
struct threadVars
{
    int column;
    int numDigits;
    int totalNums;
    int *units;
    int *carries;
    int *arr;
};

// function ran by each thread
void *addColumn(void *arg)
{
    // struct that packs all of the data we need to run the operation.
    struct threadVars *vars = (struct threadVars *)arg;

    int col = vars->column;

    // Initalize vars
    int total = 0;
    int numDigits = vars->numDigits;
    int totalNums = vars->totalNums;
    int *arr = vars->arr;
    int *units = vars->units;
    int *carries = vars->carries;

    // Add up numbers
    for (int i = 0; i < totalNums; i++)
    {
        total += arr[i];
    }

    // get carry and remainder
    int carry = total / 10;
    int remainder = total % 10;

    // set carry and remainder in array

    // should be -1 but carries has 1 more space
    carries[col] = carry;

    units[col] = remainder;

    // printf("total: %d", total);
    // printf("carries: %d\n", carries[col]);

    // free the array containing the column
    free(arr);

    // pthread_exit(0);
}

// the main adding/threading function
void adder(int **arr, int totalNums, int numDigits)
{

    // allocate arrays for the units and carries
    int *units = (int *)malloc((numDigits) * sizeof(int));
    int *carries = (int *)malloc((numDigits + 1) * sizeof(int));

    // create thread ids for all threads needed
    pthread_t thread_ids[numDigits];

    for (int i = 0; i < numDigits; i++)
    {
        // allocate memory for the data needed by each thread
        struct threadVars *local = (struct threadVars *)malloc(sizeof(struct threadVars));

        // pack the vars into the struct
        local->column = i;
        local->units = units;
        local->carries = carries;
        local->numDigits = numDigits;
        local->totalNums = totalNums;

        // hack to get around weird pointer logic in c, puts the numbers to add into their own array
        int *colArr = (int *)malloc((totalNums) * sizeof(int));

        for (int j = 0; j < totalNums; j++)
        {
            colArr[j] = arr[j][i];
        }
        local->arr = colArr;

        // finally create the thread and pass the packed struct
        pthread_create(&thread_ids[i], NULL, addColumn, local);
    }
    for (int i = 0; i < numDigits; i++)
    {
        // wait for each thread to finish.
        pthread_join(thread_ids[i], NULL);
    }

    // allocate arrays
    int *result = (int *)malloc((numDigits + 2) * sizeof(int));
    int *secondCarry = (int *)malloc((numDigits + 2) * sizeof(int));

    // add each of the rows and the carry
    for (int i = numDigits + 2; i > 1; i--)
    {
        int tot = units[i - 2] + carries[i - 1];

        result[i] = tot % 10;
        // if it is still larger than 10, add to the second carry
        secondCarry[i - 1] = tot / 10;
    }

    // get remainder
    result[1] = carries[0] % 10;
    result[0] = carries[0] / 10;

    // add any second carries
    for (int i = numDigits + 2; i >= 0; i--)
    {
        result[i] += secondCarry[i];
    }

    // prints result
    bool hasPrinted = false;
    printf("result: ");
    for (int i = 0; i < numDigits + 2; i++)
    {
        if (result[i] != 0 || hasPrinted == true)
        {
            printf("%d ", result[i]);
            hasPrinted = true;
        }
    }
    printf("\n \n");

    // free memory
    free(carries);
    free(units);
    free(secondCarry);
    free(result);
}

// runs the parser and then calls adder() to add the nums
int main()
{
    // int arr[4][4] = {
    //     {6, 9, 4, 3},
    //     {9, 4, 3, 9},
    //     {8, 8, 8, 8},
    //     {9, 9, 9, 9}};

    // int arr[12][3] = {
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9},
    //     {9, 9, 9}};

    // everything up to running the adder() function is just parsing in the input.
    printf("Enter each number in this format: 9 9 9\n");
    printf("Enter each number on a new line, hit enter a second time to add\n");
    printf("Note: all numbers need to have the same number of digits. Enter leading zeros for shorter numbers.\n \n");
    while (1)
    {
        int digits;
        int totalNums;
        int isError = 0;

        // Need these two in the C version to create the array
        printf("Enter the number of digits in each number> ");
        scanf("%d", &digits);
        if (digits > 10 || digits < 0)
        {
            printf("Error: invalid input\n");
            continue;
        }

        printf("Enter the total numbers to be added> ");
        scanf("%d", &totalNums);
        if (totalNums > 50 || totalNums < 0)
        {
            printf("Error: invalid input\n");
            continue;
        }

        printf("Enter each number to be added, with each digit separated by a space, and each number separated by a line.\n");

        int arr[totalNums][digits];
        // char line[1024], *p, *e;

        for (int i = 0; i < totalNums; i++)
        {
            printf("> ");
            for (int j = 0; j < digits; j++)
            {
                scanf("%d", &arr[i][j]);
                if (arr[i][j] < 0 || arr[i][j] > 9)
                {
                    printf("Error: invalid input\n");
                    isError = 1;
                    break;
                }
            }
            if (isError == 1)
            {
                break;
            }
        }
        if (isError == 1)
        {
            continue;
        }

        // put the array in another pointer
        int *p[totalNums];
        for (int i = 0; i < totalNums; i++)
        {
            p[i] = arr[i];
        }

        adder(p, totalNums, digits);
    }
}