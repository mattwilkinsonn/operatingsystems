# Assignment 3 for Operating Systems Class

## Matt Wilkinson, Spring 2021, Prof Japkowicz, American University

## Intro

This assignment involved using POSIX threads, mutex locks, and sephamores to simulate a teacher holding office hours on Zoom, as well as 20 of their students who alternate between studying for their midterm, and getting help from their professor.

The assignment uses the C programming language along with the -lpthread flag to include the threading parts of C.

## Main

Program starts off in main() function, which simply initalizes the (globally defined) mutexes and sephamores, and then initalizes the 20 student threads as well as the teacher thread.

## Student

The student first sleeps for a random amount of time (between 100 and 10 seconds) simulating them studying. Afterwards, they check if the waiting room of the teacher has space (an int value guarded by a mutex), and if so they add themselves to the waiting room. If not, they go back to studying.

If they did successfully add themselves to the waiting room, they then wait for the teacher to be free, using the help sephamore, which effectively forms the waiting room queue.

Once they are up to be helped, they acquire another mutex (being_helped) that synchronizes them with the teacher. They then remove themselves from the waiting room list, and then sleep for a random amount of time (between 10 and 3 seconds) simulating them being helped by the teacher.

Once they are done, they release the being_helped mutex, and then also unlock the teacher_mut mutex, which is locked by the teacher. This prevents the teacher from thinking more than 1 student came through.

Finally they release the help sephamore, allowing the next student to begin to be helped.

## Teacher

The teacher continouslly polls the help sephamore, which is effectively the waiting room queue, to see if any student is ready to be helped. If a student is holding the sephamore, and therefore the value of it is 0, then the teacher begins helping.

If there is no student waiting to be helped, the teacher just works on their own work until there is one. The teacher will also reset the number of consecutive students if they reach here.

When a student wants to be helped, the teacher first locks the teacher mutex, which prevents it from continually polling the semphamore value until they are done. Then, it waits for the student to release the being_helped mutex, which means the student has finished "being helped" and they can both move on.

The teacher takes this mutex, and then checks if the number of consecutive students is >= 3. If so, then the teacher takes a break, for a random amount of seconds between 3 and 10, simulated by a sleep. They then reset the consecutive students.
