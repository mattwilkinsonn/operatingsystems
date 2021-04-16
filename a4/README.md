# Assignment 4 for Operating Systems Class

## Matt Wilkinson, Spring 2021, Prof Japkowicz, American University

## Intro

Page-based memory management simulation, written in C. 200KB memory with 5KB pages, and a list of process changes that need to be allocated/deallocated from the memory.

## Compiling/Running Program

To compile the program, run `gcc memory.c -o memory` inside the directory. Then run `./memory` to start the program.

You can also just use the pre-compiled version in the directory by running `./memory`.

## Main

Program starts in the main() function, which takes the input from the user on how many process changes there are, and then takes the information (id, size, and status) about each change from the user. It then puts this information into a ProcessChange struct, and creates a list of these structs.

Once it has taken all of the changes from the user, it passes the list of structs as well as the total number of changes to the run() function.

## Run

The run function receives the ProcessChange structs as well as the number of them.

It first initalizes the stack, pushing all 40 frames (as there are 200KB, which divided by 5KB is 40 frames) onto the free frame stack, in descending order.

Then it initalizes the page tables, making every value -2. For the page tables the values correspond to:

-2 -> non-exisitng or terminated process;
-1 -> running process that has been swapped out of memory;
0-40 -> frame that the running process page has been allocated to in memory.

Therefore this initalizes all the page tables to the non-existing value.

The function then iterates through each process change in the list, comparing its' status to the known statuses: "new", "in", "out", and "terminated". For each of these, it then runs the corresponding function:
process_new(), process_in(), process_out(), and process_terminated(), passing the ProcessChange struct to it. If the status doesn't match any of those strings, it is an invalid status, and the run() prints an error and returns early.

After each of the process changes has run, run() finally iterates through all of the remaining free frames on the stack and prints them to the console.

## Process functions

### process_in()

process_in() first gets the number of frames the process will take up. Then it iterates over these frames, getting a free frame from the stack, and allocating that process to it, and then adding the frame to its' page table.

It also checks if the page table has the correct value beforehand (-1).

After iterating over the frames, it gets and prints out the internal fragmentation of the process.

### process_new()

process_new() is almost the same as process_in(), except the error checking changes. Instead of checking if the previous page table value was -1, it checks if it was -2.

### process_out()

process_out() first gets the number of frames to iterate over. It then iterates over these in reverse order, starting from the highest number. This is to make sure the stack gets the free frames back in the correct order.

Then it checks to make sure the page table value is not -1 or -2, making sure it is actually in memory.

It then grabs the frame from the page table, sets that frame in memory to 0 (free), sets the page table back to -1, and pushes the freed frame back onto the stack.

### process_terminated()

process_terminated() is almost the same as process_out() except for a couple changes:

First is the error checking, process_terminated only cares if the page table was already set to -2, if that is the case then there is an error.

It then gets the current frame from the page table. If this frame is -1, the process is out of memory already, so it skips freeing the memory + adding the frame to the stack, and then simply sets the page table to -2. If it isn't -1, then it still frees the memory and adds to the stack like process_out().
