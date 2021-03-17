# Assignment 2 for Operating Systems Class

## Matt Wilkinson, Spring 2021, Prof Japkowicz, American University

## Intro

Simple program which adds numbers. Uses POSIX threads for the C verison, and the Runnable interface for the Java version.

## C version

Starts out in main(), which parses in the input from the user to create the array.
Had to ask for the number of digits as well as the total numbers to be able to create the array in C, did not need to do that in Java.

passes adder() the array, which creates threads for each column to add. Each thread runs the addColumn() fn. Also packs a struct for each thread to give it access to the variables it needs.

Each thread gets passed a reference to the units and carries arrays, as well as a column to put their result in. This is logically thread-safe but is practically super dangerous, but not really worth implementing a mutex for every value in each array for an assignment that isn't going to change.

Once the threads are finished, adds the numbers together (and checks for a second carry), and then prints the result.

## Java version

Basically the same logic as the C version, parsing in the main() function, and then creates the threads and adds in the adder() function, which runs the AddThread class in each thread. A lot less complicated as parsing in Java is easier, and I can just pass variables to the thread instead of having to pass a struct.

Uses global arrays for units and carries (the ones that the threads put their results into) because you can't pass references in Java.
