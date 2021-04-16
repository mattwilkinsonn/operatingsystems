#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

// stack for free frame locations.
int MAXSIZE = 40;
int stack[40];
int top = -1;

// methods for stack.
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

// end stack stuff

// struct that holds each process "change"
struct ProcessChange
{
    int process_id;
    int size;
    char *status;
};

// int num_inputs = 15;

// struct ProcessChange input[15] = {
// {1, 22, "new"}, {2, 30, "new"}, {1, 22, "out"}, {3, 15, "new"}, {4, 8, "new"}, {2, 30, "out"}, {1, 22, "in"}, {3, 15, "terminated"}, {2, 30, "in"}, {5, 25, "new"}, {6, 19, "new"}, {7, 5, "new"}, {5, 25, "out"}, {4, 8, "terminated"}, {8, 19, "new"}};

// the fake memory frames. 200/5 = 40, so there are 40 frames
int memory[40] = {0};

// the page tables for each process. -2 denotes a non existing or terminated process.
int pageTables[40][40] = {-2};

// helper fn to get the number of frames a process takes up.
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

// helper fn to get the internal fragmentation of a process.
int get_internal_fragmentation(int process_size)
{
    int mod = process_size % 5;
    if (mod == 0)
        return 0;

    return 5 - (process_size % 5);
}

// run on a process in.
void process_in(struct ProcessChange process)
{
    // get frames
    int num_frames = get_num_frames(process.size);
    printf("Page Table for in process %d:\n", process.process_id);
    for (int i = 0; i < num_frames; i++)
    {
        // error checking: if pagetable is not set to -1 (denoting a running process not in memory), this is an invalid in.
        if (pageTables[process.process_id][i] != -1)
        {
            printf("invalid in: Process %d page %d is not set to -1\n", process.process_id, i + 1);
            return;
        }

        // get the next free frame off the stack.
        int frame = pop();

        // set the memory to be held by that process.
        memory[frame] = process.process_id;

        // set the page table to correspond to the frame.
        pageTables[process.process_id][i] = frame;

        // print to console.
        printf("Process page %d allocated to memory frame %d\n", i + 1, pageTables[process.process_id][i] + 1);
    }

    // get the internal fragmentation of the process after finished allocating.
    int internal_fragmentation = get_internal_fragmentation(process.size);
    printf("Process has wasted %d KB due to internal fragmentation\n", internal_fragmentation);
}

// run on a process new. This is almost the same as process_in above, except for 1 key difference.
// Read comments for process_in to understand the other parts.
void process_new(struct ProcessChange process)
{

    int num_frames = get_num_frames(process.size);
    printf("Page Table for new process %d:\n", process.process_id);
    for (int i = 0; i < num_frames; i++)
    {
        // error checking: if pagetable is not set to -2 (denoting a NOT running process), this is an invalid new.
        if (pageTables[process.process_id][i] != -2)
        {
            printf("invalid new: Process %d page %d is not set to -2\n", process.process_id, i) + 1;
        }

        int frame = pop();
        memory[frame] = process.process_id;
        pageTables[process.process_id][i] = frame;

        printf("Process page %d allocated to memory frame %d\n", i + 1, pageTables[process.process_id][i] + 1);
    }

    int internal_fragmentation = get_internal_fragmentation(process.size);
    printf("Process has wasted %d KB due to internal fragmentation\n", internal_fragmentation);
}

// run on a process out.
void process_out(struct ProcessChange process)
{
    // get frames.
    int num_frames = get_num_frames(process.size);
    // going from high->low here so the stack gets the frames in the right order.
    for (int i = num_frames - 1; i >= 0; i--)
    {
        // error checking: if pagetable set to -1 or -2. the process is not in memory making this out invalid.
        if (pageTables[process.process_id][i] == -1 || pageTables[process.process_id][i] == -2)
        {
            printf("invalid out: Process %d page %d is not in memory\n", process.process_id, i + 1);
            return;
        }

        // get the current frame from pagetable.
        int frame = pageTables[process.process_id][i];
        // set memory back to 0.
        memory[frame] = 0;

        // set page table to -1 (running process not in memory)
        pageTables[process.process_id][i] = -1;

        // push freed frame onto stack.
        push(frame);
    }
    printf("Process %d's page table entries have been set to -1\n", process.process_id);
}

// Same as process_out except for a couple differences.
void process_terminated(struct ProcessChange process)
{
    int num_frames = get_num_frames(process.size);
    for (int i = num_frames - 1; i >= 0; i--)
    {
        // error checking: if pagetable set to -2. the process is already terminated making this invalid.
        if (pageTables[process.process_id][i] == -2)
        {
            printf("invalid termination: Process %d page %d is already terminated\n", process.process_id, i + 1);
            return;
        }
        int frame = pageTables[process.process_id][i];

        // if frame is -1, it is already out of memory so we don't need to push it to the stack or free the memory.
        if (frame != -1)
        {
            memory[frame] = 0;
            push(frame);
        }

        // set page table to terminated value (-2)
        pageTables[process.process_id][i] = -2;
    }
    printf("Process %d's page table has been deleted\n", process.process_id);
}

// the function that runs the actual simulation. Takes the parsed ProcessChange list and the number of changes in that list.
void run(struct ProcessChange *input, int num_inputs)
{
    // initalize the stack. Push all 40 frames onto it, in descending order.
    for (int i = 40; i >= 0; i--)
    {
        push(i);
    }

    // initalize the page tables. Set all of them to the terminated value (-2).
    for (int i = 0; i < 40; i++)
    {
        for (int j = 0; j < 40; j++)
        {
            pageTables[i][j] = -2;
        }
    }

    // iterate through the process changes.
    for (int i = 0; i < num_inputs; i++)
    {
        printf("\nBegin process %d -> %s, with size %d KB\n", input[i].process_id, input[i].status, input[i].size);
        struct ProcessChange process = input[i];

        // compare status string to know which fn to run.
        if (strcmp(process.status, "new") == 0)
        {
            process_new(process);
        }
        else if (strcmp(process.status, "in") == 0)
        {
            process_in(process);
        }
        else if (strcmp(process.status, "out") == 0)
        {
            process_out(process);
        }
        else if (strcmp(process.status, "terminated") == 0)
        {
            process_terminated(process);
        }
        else
        {
            // invalid status input, error
            printf("\nError: process %d status %s is invalid status\n", process.process_id, process.status);
            return;
        }
    }

    // once done iterating through the changes, pop off all free frames from the stack and print to console.
    printf("\nFree Frames:\n");
    while (!isempty())
    {
        int free_frame = pop();
        printf("%d\n", free_frame + 1);
    }
}

// takes the input from the user, and then runs the run() fn with the parsed structs
int main()
{
    // loop so you can keep running sims as long as you want.
    while (1)
    {
        int num_inputs;
        printf("Enter the total number of process changes> ");
        scanf("%d", &num_inputs);

        printf("For each process change, you will be asked for the process id, the size of the process, and then what status the process is moving to.\n");

        // allocate memory for structs
        struct ProcessChange *input = (struct ProcessChange *)malloc(sizeof(struct ProcessChange) * num_inputs);

        // get each process change from user.
        for (int i = 0; i < num_inputs; i++)
        {
            int id;
            int size;
            char status[10];

            printf("\nProcess change %d\n", i + 1);

            printf("Enter process id> ");
            scanf("%d", &id);

            printf("Enter process size> ");
            scanf("%d", &size);

            printf("Enter process status> ");
            scanf("%s", status);

            // allocate string to heap so it doesn't get obliterated on the stack
            char *statusptr = (char *)malloc(sizeof(status));
            strcpy(statusptr, status);

            // create struct
            struct ProcessChange process =
                {
                    id,
                    size,
                    statusptr};
            printf("Adding process change: process %d -> status %s, with size %d KB\n", id, statusptr, size);

            // add struct to list
            input[i] = process;
        }

        // finished adding structs to list, pass the list and the number of changes to the run() fn
        run(input, num_inputs);

        // free the struct list memory.
        free(input);
    }
}