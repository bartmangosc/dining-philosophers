#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>

#define NUM_PHIL 5
#define NUM_FORKS 5

// Dining philosophers problem (using processes) - possible starvation

// States of the philosophers: thinking, hungry, eating
typedef enum { THINKING, HUNGRY, EATING} State;

struct shared_data {
    int fork_id[NUM_FORKS];
    State philosophers_states[NUM_PHIL];
    int meals_eaten[NUM_PHIL];
};

struct shared_data *shared;
int forks_sem[NUM_FORKS];
int table_sem;
FILE *log_file;


void sem_op(int sem_id, int sem_num, int op) {
    struct sembuf operation = {sem_num, op, 0};
    semop(sem_id, &operation, 1);
}


void think(int i) {
    int think_time = rand() % (50 - 5 + 1) + 5;
    shared->philosophers_states[i] = THINKING;
    think_time *= 100000;
    usleep(think_time);
}


void eat(int i) {
    int eat_time = rand() % (50 - 5 + 1) + 5;
    shared->philosophers_states[i] = EATING;
    shared->meals_eaten[i]++;
    eat_time *= 100000;
    usleep(eat_time);
}


void philosopher(int nr) {
    while (1) {
        think(nr);
	
	shared->philosophers_states[nr] = HUNGRY;
	sem_op(table_sem, 0, -1);
	
        sem_op(forks_sem[nr], 0, -1);
        sem_op(forks_sem[(nr + 1) % NUM_FORKS], 0, -1);
        
        sem_op(table_sem, 0, 1);

        eat(nr);

        sem_op(forks_sem[nr], 0, 1);
        sem_op(forks_sem[(nr + 1) % NUM_FORKS], 0, 1);

    }
}


void display_states() {
    fprintf(log_file, "\nState of the philosophers (using processes) - possible starvation:\n");
    fflush(log_file);
    while (1) {
        printf("\nState of the philosophers (using processes) - possible starvation:\n");
        for (int i = 0; i < NUM_PHIL; i++) {
            const char *state_str;
            switch (shared->philosophers_states[i]) {
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
            printf("Philospher %d: %s, number of meals eaten: %d\n", i, state_str, shared->meals_eaten[i]);
            fprintf(log_file, "Philosopher %d: %s, number of meals eaten: %d\n", i, state_str, shared->meals_eaten[i]);
            fflush(log_file);
        }
        printf("\n");
        fprintf(log_file, "\n");
        fflush(log_file);
        sleep(1);
    }
}


int main() {
    srand(time(NULL));

    for (int i = 0; i < NUM_FORKS; i++) {
        forks_sem[i] = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
        semctl(forks_sem[i], 0, SETVAL, 1);
    }

    shared = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for (int i = 0; i < NUM_FORKS; i++) {
        shared->fork_id[i] = 1;
    }
    
    table_sem = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    semctl(table_sem, 0, SETVAL, 1);

    log_file = fopen("dpp_starvation_log.txt", "w");
    if (!log_file) {
        perror("Unable to open file");
        return 1;
    }

    pid_t pids[NUM_PHIL];
    for (int i = 0; i < NUM_PHIL; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            philosopher(i);
            exit(0);
        }
    }

    display_states();

    for (int i = 0; i < NUM_PHIL; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    for (int i = 0; i < NUM_FORKS; i++) {
        semctl(forks_sem[i], 0, IPC_RMID);
    }
    
    semctl(table_sem, 0, IPC_RMID);
    munmap(shared, sizeof(struct shared_data));
    
    fclose(log_file);
    
    return 0;
}
