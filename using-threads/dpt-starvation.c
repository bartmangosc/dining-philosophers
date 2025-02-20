#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PHIL 5
#define NUM_FORKS 5

// Dining philosophers problem (using threads) - possible starvation

// States of the philosophers: thinking, hungry, eating
typedef enum { THINKING, HUNGRY, EATING } State;

struct {
    pthread_mutex_t mut;
    pthread_cond_t ok[NUM_PHIL];
    int fork_id[NUM_FORKS];
} f = {PTHREAD_MUTEX_INITIALIZER, {PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER}, {1, 1, 1, 1, 1}};

pthread_t state_thread;
pthread_t philosopher_threads_table[NUM_PHIL];
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


void *philosopher(void *arg) {
    int nr = *(int *)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&f.mut);

        philosophers_states[nr] = HUNGRY;
        while (f.fork_id[nr] + f.fork_id[(nr + 1) % NUM_FORKS] < 2) {
            pthread_cond_wait(&f.ok[nr], &f.mut);
        }
        f.fork_id[nr] = 0;
        f.fork_id[(nr + 1) % NUM_FORKS] = 0;

        pthread_mutex_unlock(&f.mut);

        eat(nr);

        pthread_mutex_lock(&f.mut);

        f.fork_id[nr] = 1;
        f.fork_id[(nr + 1) % NUM_FORKS] = 1;
        pthread_cond_signal(&f.ok[(nr - 1 + NUM_PHIL) % NUM_PHIL]);
        pthread_cond_signal(&f.ok[(nr + 1) % NUM_PHIL]);

        pthread_mutex_unlock(&f.mut);

        think(nr);
    }
    return NULL;
}


void *display_states(void *arg) {
    fprintf(log_file, "\nState of the philosophers (using threads) - possible starvation:\n");
    fflush(log_file);
    while (1) {
        printf("\nState of the philosophers (using threads) - possible starvation:\n");
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
    log_file = fopen("dpt_starvation_log.txt", "w");
    if (!log_file) {
        perror("Unable to open file");
        return 1;
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
