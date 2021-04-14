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

int main()
{
    for (int i = 40; i >= 0; i--)
    {
        push(i);
    }

    for (int i = 0; i < num_inputs; i++)
    {
        struct ProcessChange process = input[i];
        switch (process.status)
        {
        case new:
        case in:
            process_new_or_in(process);
            break;
        case out:
            process_out(process);
            break;
        case terminated:
        default:
            return 1;
        }
        printf("process %d with size %d status is %d\n", input[i].process_id, input[i].size, input[i].status);
    }
}

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

void process_add(struct ProcessChange process)
{
    int num_frames = get_num_frames(process.size);
    for (int i = 0; i < num_frames; i++)
    {
        int frame = pop();
        memory[frame] = process.process_id;
        pageTables[process.process_id][i] = frame;
    }
}

void process_new_or_in(struct ProcessChange process)
{
    int num_frames = get_num_frames(process.size);
    process_add(process);
    printf("Page Table for new process %d:\n", process.process_id);
    for (int i = 0; i < num_frames; i++)
    {
        printf("Process page %d allocated to memory frame %d", i, pageTables[process.process_id][i]);
    }
}

void process_out(struct ProcessChange process)
{
    int num_frames = get_num_frames(process.size);
    for (int i = num_frames; i >= 0; i--)
    {
        int frame = pageTables[process.process_id][i];
        memory[frame] = 0;
        pageTables[process.process_id][i] = -1;
        push(frame);
    }
    printf("Process %d's page table entires have been set to -1", process.process_id);
}

void process_terminated(struct ProcessChange process)
{
    int num_frames = get_num_frames(process.size);
}