#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/wait.h>

#define max_clients_count 30 // max number of clients

pid_t child_pids[max_clients_count];

int memory_fd;

char sem_name[] = "barbershop_semaphore";	// semaphore name
sem_t *sem_addr; 	// address of semaphore


typedef struct {
    int hairdresser_is_free;
    int in_line;
} barbershop;

barbershop* maxec_barbershop;

void closing(int id) {
    sem_close(sem_addr);
    shm_unlink(sem_name);
    exit(-1);
}

int main(){
    signal(SIGINT,closing);
    signal(SIGTERM,closing);

    int num_of_clients;
    printf("Write number of clients (1 to %d)\n", max_clients_count);
    scanf("%d", &num_of_clients);

    if (!(sem_addr = sem_open(sem_name, O_CREAT, 0666, 1))) {
        perror("ERROR: Failed to create semaphore!");
        return -1;
    }
    
    shm_unlink(sem_name);
    if ((memory_fd = shm_open(sem_name, O_CREAT | O_EXCL | O_RDWR, 0666)) == -1) {
        perror("ERROR: Failed to create shared memory!");
        return -1;
    } else {
        printf("Shared memory %s successfully created!\n", sem_name);
    }

    if (ftruncate(memory_fd, sizeof(barbershop)) == -1) {
        perror("ERROR: Memory error(ftruncate)");
        return 1;
    } else {
        printf("Memory successfully created! Size of memory: %ld\n", sizeof(barbershop));
    }
    maxec_barbershop = mmap(0, sizeof (barbershop), PROT_READ|PROT_WRITE, MAP_SHARED, memory_fd, 0);
    maxec_barbershop->hairdresser_is_free = 1;
    maxec_barbershop->in_line = 0;

    // start cycle of hairdressing
    for(int i = 1; i <= num_of_clients; i++){
        int result = fork();
        if (result < 0){
            printf("ERROR: while forking!");
            exit(-1);
        } else if (result == 0){
            srand(time(NULL));
            maxec_barbershop = mmap(0, sizeof (barbershop), PROT_READ|PROT_WRITE, MAP_SHARED, memory_fd, 0);

            int* hairdresser_is_free = &maxec_barbershop->hairdresser_is_free;
            int* in_line = &maxec_barbershop->in_line;
            bool haircut_started = false;

            ++(*in_line);
            printf("....Client number %d came to barbershop! He is number %d in line!....\n", i, *in_line);
            while(!haircut_started){
                //waiting line
                if(sem_wait(sem_addr) == -1) {
                    perror("ERROR: sem_wait!");
                    exit(-1);
                }

                // if hairdresser is free
                if (*hairdresser_is_free){
                    printf("Client %d haircut has started! In line: %d\n", i, *in_line - 1);
                    haircut_started = true;
                    *in_line = *in_line - 1;
                    *hairdresser_is_free = 0;
                }

                if(sem_post(sem_addr) == -1) {
                    perror("ERROR: sem_post!");
                    exit(-1);
                }
            }
            //haircut in progress
            sleep(5 + 10 * ((double)(abs(rand())) / RAND_MAX));

            if(sem_wait(sem_addr) == -1) {
                perror("ERROR: sem_wait!");
                exit(-1);
            }

            //ending haircut and maybe hairdresser has a free time

            printf("Client number %d haircut finished!\n", i);
            *hairdresser_is_free = 1;

            if (*in_line == 0) {
                printf("    [Hairdresser goes to sleep!]\n");
            }

            if(sem_post(sem_addr) == -1) {
                perror("ERROR: sem_post!");
                exit(-1);
            }

            close(memory_fd);
            exit(0);
        }
        sleep(5 + 10 * ((double)(abs(rand())) / RAND_MAX));
        child_pids[i] = result;
    }
    for (int i = 0; i < num_of_clients; ++i) {
        int status;
        waitpid(child_pids[i], &status, 0);
    }
    if(sem_close(sem_addr) == -1) {
        perror("ERROR: sem_close!");
    }
    if(sem_unlink(sem_name) == -1) {
        perror("ERROR: sem_unlink!");
    }
    if (shm_unlink(sem_name) == -1){
        perror("ERROR: while closing shared memory!");
    }
    return 0;

}
