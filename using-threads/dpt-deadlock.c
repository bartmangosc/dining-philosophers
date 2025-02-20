#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PHIL 5
#define NUM_FORKS 5

// Dining philosophers problem (using threads) - possible deadlock

// States of the philosophers: thinking, hungry, eating
typedef enum { THINKING, HUNGRY, EATING } State;

void *philosopher(void *arg);
void *display_states(void *arg);
pthread_t philosopher_threads_table[NUM_PHIL];
pthread_t state_thread;
sem_t _fork[NUM_FORKS];

State philosophers_states[NUM_PHIL];
int meals_eaten[NUM_PHIL] = {0};
FILE *log_file;


void think(int i) {
    int think_time = rand() % (5 - 2 + 1) + 2;
    philosophers_states[i] = THINKING;
    sleep(think_time);
}


void eat(int i) {
    int eat_time = rand() % (4 - 1 + 1) + 1;
    philosophers_states[i] = EATING;
    meals_eaten[i]++;
    sleep(eat_time);
}


void hungry(int i) {
    philosophers_states[i] = HUNGRY;
}


void *philosopher(void *arg) {
    int phil_no = *(int *)arg;
    free(arg);

    while (1) {
        think(phil_no);
        hungry(phil_no);

        sem_wait(&_fork[phil_no]);
	    sleep(1);
        sem_wait(&_fork[(phil_no + 1) % NUM_FORKS]);

        eat(phil_no);

        sem_post(&_fork[(phil_no + 1) % NUM_FORKS]);
        sem_post(&_fork[phil_no]);
    }
}


void *display_states(void *arg) {
    fprintf(log_file, "\nState of the philosophers (using threads) - possible deadlock:\n");
    fflush(log_file);
    while (1) {
        printf("\nState of the philosophers (using threads) - possible deadlock:\n");
        for (int i = 0; i < NUM_PHIL; i++) {
            const char *state_str;
            switch (philosophers_states[i]) {
                case THINKING:
                    state_str = "THINKING";
                    break;
                case HUNGRY:
                    state_str = "HUNGRY";
                    break;
                case EATING:
                    state_str = "EATING";
                    break;
            }
            printf("Philosopher %d: %s, number of meals eaten: %d\n", i, state_str, meals_eaten[i]);
            fprintf(log_file, "Philosopher %d: %s, number of meals eaten: %d\n", i, state_str, meals_eaten[i]);
            fflush(log_file);
        }
        printf("\n");
        fprintf(log_file, "\n");
        fflush(log_file);
        sleep(1);
    }
    return NULL;
}


int main() {
    log_file = fopen("dpt_deadlock_log.txt", "w");
    if (!log_file) {
        perror("Unable to open file");
        return 1;
    }

    for (int i = 0; i < NUM_FORKS; i++) {
        sem_init(&_fork[i], 0, 1);
    }

    for (int i = 0; i < NUM_PHIL; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&philosopher_threads_table[i], NULL, philosopher, arg);
    }

    pthread_create(&state_thread, NULL, display_states, NULL);

    for (int i = 0; i < NUM_PHIL; i++) {
        pthread_join(philosopher_threads_table[i], NULL);
    }

    pthread_join(state_thread, NULL);
    
    fclose(log_file);
    
    return 0;
}
