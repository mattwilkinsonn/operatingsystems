#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

sem_t help;
sem_t waiting_room;
pthread_mutex_t waiting_room_free_lock;
int waiting_room_free = 2;
pthread_mutex_t being_helped;
int num_consecutive_students = 0;
pthread_mutex_t teacher_mut;

struct student_vars
{
    int num;
    int lalala;
};

struct teacher_vars
{
};

int generateRandomSleep(int upper, int lower)
{
    return (rand() % (upper - lower + 1)) + lower;
}

void *student(void *arg)
{
    struct student_vars *vars = (struct student_vars *)arg;
    int id = vars->num;

    while (1)
    {
        // begin by studying for random time.
        int random_sleep = generateRandomSleep(100, 10);
        printf("Student %d studying for %d seconds\n", id, random_sleep);
        sleep(random_sleep);

        // needs help. checks if the waiting room has space.
        pthread_mutex_lock(&waiting_room_free_lock);
        bool isWait = false;
        printf("Student %d checking if waiting room has space\n", id);
        if (waiting_room_free > 0)
        {
            printf("Adding student %d to waiting room\n", id);
            waiting_room_free -= 1;
            isWait = true;
        }
        pthread_mutex_unlock(&waiting_room_free_lock);
        // in waiting room, wait for teacher.
        if (isWait)
        {
            printf("Student %d in waiting room, waiting for teacher\n", id);
            // waiting room 'queue'
            sem_wait(&help);
            printf("Student %d next to be helped\n", id);

            // sync with teacher
            pthread_mutex_lock(&being_helped);

            // exit waiting room, freeing up spot
            pthread_mutex_lock(&waiting_room_free_lock);
            waiting_room_free += 1;
            pthread_mutex_unlock(&waiting_room_free_lock);
            printf("Student %d left waiting room\n", id);

            // work with teacher for random amount of time
            int random_sleep = generateRandomSleep(10, 3);
            printf("Student %d being helped for %d seconds\n", id, random_sleep);
            sleep(random_sleep);

            printf("Student %d done being helped\n", id);

            // allows teacher to check num students etc
            pthread_mutex_unlock(&being_helped);

            // tells teacher they are done
            pthread_mutex_unlock(&teacher_mut);

            // leave the queue
            sem_post(&help);
        }
        else
        {
            printf("Waiting room full, Student %d will go back to studying\n", id);
        }
    }
}

void *teacher(void *arg)
{
    bool teacher_working = false;
    int help_val;
    while (1)
    {
        sem_getvalue(&help, &help_val);
        // continously polls this value until student has taken it
        if (help_val == 0)
        {
            // Note: unlocked by student when they are done
            pthread_mutex_lock(&teacher_mut);
            printf("Teacher begin helping student\n");

            num_consecutive_students += 1;

            // grab from student, prevents next one from going until checked consecutive students
            pthread_mutex_lock(&being_helped);
            printf("Teacher done helping student\n");
            if (num_consecutive_students >= 3)
            {
                int random_sleep = generateRandomSleep(10, 3);
                printf("Teacher consecutive students: %d, Teacher taking break for %d seconds\n", num_consecutive_students, random_sleep);
                sleep(random_sleep);
                num_consecutive_students = 0;
            }
            pthread_mutex_unlock(&being_helped);
            teacher_working = false;
        }
        // boolean so it only runs once until reset
        else if (!teacher_working)
        {
            pthread_mutex_lock(&waiting_room_free_lock);
            // no students waiting, reset consecutive and say working on own work
            if (waiting_room_free >= 2)
            {
                num_consecutive_students = 0;
                printf("Teacher working on their own work\n");
                teacher_working = true;
            }
            pthread_mutex_unlock(&waiting_room_free_lock);
        }
    }
}

int main()
{
    printf("Begin program\n");

    // Init mutexes and sephamores, seed random value, create threads and join

    int num_students = 20;
    pthread_t student_ids[20];
    pthread_t teacher_id;
    srand(time(0));

    if (pthread_mutex_init(&waiting_room_free_lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&being_helped, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&teacher_mut, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    sem_init(&help, 0, 1);

    printf("Creating student threads\n");

    for (int i = 0; i < num_students; i++)
    {
        struct student_vars *local = (struct student_vars *)malloc(sizeof(struct student_vars));

        local->num = i;
        pthread_create(&student_ids[i], NULL, student, local);
    }

    struct teacher_vars *local = (struct teacher_vars *)malloc(sizeof(struct teacher_vars));

    pthread_create(&teacher_id, NULL, teacher, local);

    pthread_join(teacher_id, NULL);

    for (int i = 0; i < num_students; i++)
    {
        pthread_join(student_ids[i], NULL);
    }

    printf("Threads ended\n");
}