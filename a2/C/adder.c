#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

struct threadVars
{
    int column;
    int numDigits;
    int totalNums;
    int *units;
    int *carries;
    int *arr;
};

void *addColumn(void *arg)
{
    struct threadVars *vars = (struct threadVars *)arg;

    int col = vars->column;
    // printf("column: %d", vars->column);
    int total = 0;
    int numDigits = vars->numDigits;
    int totalNums = vars->totalNums;
    int *arr = vars->arr;
    int *units = vars->units;
    int *carries = vars->carries;

    for (int i = 0; i < totalNums; i++)
    {
        total += arr[i];
    }
    int carry = total / 10;
    int remainder = total % 10;

    // should be -1 but carries has 1 more space
    carries[col] = carry;

    units[col] = remainder;

    // printf("total: %d", total);
    // printf("carries: %d\n", carries[col]);
    free(arr);

    // pthread_exit(0);
}

void adder(int **arr, int totalNums, int numDigits)
{

    int *units = (int *)malloc((numDigits) * sizeof(int));
    int *carries = (int *)malloc((numDigits + 1) * sizeof(int));

    pthread_t thread_ids[numDigits];

    for (int i = 0; i < numDigits; i++)
    {
        struct threadVars *local = (struct threadVars *)malloc(sizeof(struct threadVars));

        local->column = i;
        local->units = units;
        local->carries = carries;
        local->numDigits = numDigits;
        local->totalNums = totalNums;

        int *colArr = (int *)malloc((totalNums) * sizeof(int));

        for (int j = 0; j < totalNums; j++)
        {
            colArr[j] = arr[j][i];
        }
        local->arr = colArr;

        pthread_create(&thread_ids[i], NULL, addColumn, local);
    }
    for (int i = 0; i < numDigits; i++)
    {
        pthread_join(thread_ids[i], NULL);
    }

    // for (int i = 0; i < numDigits + 1; i++)
    // {
    //     printf("carries: %d", carries[i]);
    // }
    // printf("\n");

    // for (int i = 0; i < numDigits; i++)
    // {
    //     printf("units: %d", units[i]);
    // }
    // printf("\n");

    int *result = (int *)malloc((numDigits + 2) * sizeof(int));
    int *secondCarry = (int *)malloc((numDigits + 2) * sizeof(int));

    for (int i = numDigits + 2; i > 1; i--)
    {
        int tot = units[i - 2] + carries[i - 1];

        result[i] = tot % 10;
        secondCarry[i - 1] = tot / 10;
    }
    result[1] = carries[0] % 10;
    result[0] = carries[0] / 10;

    // add any second carries
    for (int i = numDigits + 2; i >= 0; i--)
    {
        result[i] += secondCarry[i];
    }

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

    free(carries);
    free(units);
    free(secondCarry);
    free(result);
}

// not sure how to take input
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

    printf("Enter each number in this format: 9 9 9\n");
    printf("Enter each number on a new line, hit enter a second time to add\n");
    printf("Note: all numbers need to have the same number of digits. Enter leading zeros for shorter numbers.\n \n");
    while (1)
    {
        int digits;
        int totalNums;
        int isError = 0;
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

        int *p[totalNums];
        for (int i = 0; i < totalNums; i++)
        {
            p[i] = arr[i];
        }

        adder(p, totalNums, digits);
    }
}