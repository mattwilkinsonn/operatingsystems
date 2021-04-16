#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int MAXSIZE = 40;
int stack[40];
int top = -1;

int isempty()
{

    if (top == -1)
        return 1;
    else
        return 0;
}

int isfull()
{

    if (top == MAXSIZE)
        return 1;
    else
        return 0;
}

int peek()
{
    return stack[top];
}

int pop()
{
    int data;

    if (!isempty())
    {
        data = stack[top];
        top = top - 1;
        return data;
    }
    else
    {
        printf("Could not retrieve data, Stack is empty.\n");
    }
}

int push(int data)
{

    if (!isfull())
    {
        top = top + 1;
        stack[top] = data;
    }
    else
    {
        printf("Could not insert data, Stack is full.\n");
    }
}

struct ProcessChange
{
    int process_id;
    int size;
    enum
    {
        new,
        in,
        out,
        terminated
    } status;
};

int num_inputs = 15;

struct ProcessChange input[15] = {
    {1, 22, new}, {2, 30, new}, {1, 22, out}, {3, 15, new}, {4, 8, new}, {2, 30, out}, {1, 22, in}, {3, 15, terminated}, {2, 30, in}, {5, 25, new}, {6, 19, new}, {7, 5, new}, {5, 25, out}, {4, 8, terminated}, {8, 19, new}};

int memory[40] = {0};

int pageTables[10][10] = {-2};

int get_num_frames(int process_size)
{
    int num_frames;
    if (process_size % 5 == 0)
    {
        return process_size / 5;
    }
    else
    {
        return (process_size / 5) + 1;
    }
}

int get_internal_fragmentation(int process_size)
{
    int mod = process_size % 5;
    if (mod == 0)
        return 0;

    return 5 - (process_size % 5);
}

void process_in(struct ProcessChange process)
{

    int num_frames = get_num_frames(process.size);
    printf("Page Table for in process %d:\n", process.process_id);
    for (int i = 0; i < num_frames; i++)
    {
        if (pageTables[process.process_id][i] != -1)
        {
            printf("invalid in: Process %d page %d is not set to -1\n", process.process_id, i);
            return 1;
        }

        int frame = pop();
        memory[frame] = process.process_id;
        pageTables[process.process_id][i] = frame;

        printf("Process page %d allocated to memory frame %d\n", i, pageTables[process.process_id][i]);
    }

    int internal_fragmentation = get_internal_fragmentation(process.size);
    printf("Process has wasted %d KB due to internal fragmentation\n", internal_fragmentation);
}

void process_new(struct ProcessChange process)
{

    int num_frames = get_num_frames(process.size);
    printf("Page Table for new process %d:\n", process.process_id);
    for (int i = 0; i < num_frames; i++)
    {
        if (pageTables[process.process_id][i] != -2)
        {
            printf("invalid new: Process %d page %d is not set to -2\n", process.process_id, i);
        }

        int frame = pop();
        memory[frame] = process.process_id;
        pageTables[process.process_id][i] = frame;

        printf("Process page %d allocated to memory frame %d\n", i, pageTables[process.process_id][i]);
    }

    int internal_fragmentation = get_internal_fragmentation(process.size);
    printf("Process has wasted %d KB due to internal fragmentation\n", internal_fragmentation);
}

void process_out(struct ProcessChange process)
{
    int num_frames = get_num_frames(process.size);
    for (int i = num_frames - 1; i >= 0; i--)
    {
        if (pageTables[process.process_id][i] == -1 || pageTables[process.process_id][i] == -2)
        {
            printf("invalid out: Process %d page %d is not in memory\n", process.process_id, i);
        }

        int frame = pageTables[process.process_id][i];
        memory[frame] = 0;
        pageTables[process.process_id][i] = -1;
        push(frame);
    }
    printf("Process %d's page table entries have been set to -1\n", process.process_id);
}

void process_terminated(struct ProcessChange process)
{
    int num_frames = get_num_frames(process.size);
    for (int i = num_frames - 1; i >= 0; i--)
    {

        if (pageTables[process.process_id][i] == -2)
        {
            printf("invalid termination: Process %d page %d is already terminated\n", process.process_id, i);
        }

        int frame = pageTables[process.process_id][i];
        memory[frame] = 0;
        pageTables[process.process_id][i] = -2;
        push(frame);

        printf("Process %d's page table has been deleted\n", process.process_id);
    }
}

char *match_status_to_str(struct ProcessChange process)
{
    switch (process.status)
    {
    case new:
        return "new";
    case in:
        return "in";
    case out:
        return "out";
    case terminated:
        return "terminated";
    default:
        return "err";
    }
}

int main()
{
    for (int i = 40; i >= 0; i--)
    {
        push(i);
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            pageTables[i][j] = -2;
        }
    }

    for (int i = 0; i < num_inputs; i++)
    {
        printf("\nBegin process %d -> %s, with size %d KB\n", input[i].process_id, match_status_to_str(input[i]), input[i].size);
        struct ProcessChange process = input[i];
        switch (process.status)
        {
        case new:
            process_new(process);
            break;
        case in:
            process_in(process);
            break;
        case out:
            process_out(process);
            break;
        case terminated:
            process_terminated(process);
            break;
        default:
            return 1;
        }
    }

    printf("\nFree Frames:\n");
    while (!isempty())
    {
        int free_frame = pop();
        printf("%d\n", free_frame);
    }
}