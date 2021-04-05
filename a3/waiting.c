#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

sem_t help;
pthread_mutex_t waiting_room_free_lock;
int waiting_room_free = 2;
pthread_mutex_t being_helped;
int num_consecutive_students = 0;

struct student_vars
{
    int num;
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
    while (1)
    {
        sleep(generateRandomSleep(100, 10));
        pthread_mutex_lock(&waiting_room_free_lock);
        bool isWait = false;
        if (waiting_room_free > 0)
        {
            waiting_room_free -= 1;
            isWait = true;
        }
        pthread_mutex_unlock(&waiting_room_free_lock);
        if (isWait)
        {
            sem_wait(&help);
            pthread_mutex_lock(&being_helped);
            printf("Student being helped");
            sleep(generateRandomSleep(60, 10));
            printf("Student done being helped");
            pthread_mutex_unlock(&being_helped);
            sem_post(&help);
        }
    }
}

void *teacher(void *arg)
{
    while (1)
    {
        printf("Teacher thread begin");

        int help_val;
        sem_getvalue(&help, &help_val);
        if (help_val == 0)
        {
            num_consecutive_students += 1;

            pthread_mutex_lock(&being_helped);
            if (num_consecutive_students >= 3)
            {
                printf("Teacher taking break");
                sleep(generateRandomSleep(100, 10));
            }
            pthread_mutex_unlock(&being_helped);
        }
    }
}

int main()
{
    printf("Begin program");

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
    sem_init(&help, 0, 1);

    printf("Creating student threads");

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

    printf("Threads ended");
}