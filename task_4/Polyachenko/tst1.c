/* Задание: запустить данную программу и убедиться, что последовательность вывода порождаемых потоков нарушается: чем дольше они работают,
тем "сильнее разбегаются" значения, выведенные ими в stdout. Такое поведение-следствие работы планировщика.
Замечание: компилировать нужно с опцией -pthread, а если Вы на Windows и компилируете MinGW, то -mthread.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>

// N = 720720 = 2*3*2*5*7*2*3*11*13*2 - can be used as N_points - divides up to 16.

/*
 *     global memory used instead of shared. It was easier to implement (at least for me) and it has nothing to do with the main goal of this task.
 */
double *shm_res;
long long N_points;
int N_threads;
sem_t *sem_final_res;
sem_t *sem_thread_launch;
int th_k = 0;
int dgb;

void print_usage(char *exe_name);
void* compute_pi(void *arg);
double my_rnd(void);
double my_rnd_ab(double a, double b);
void my_swap(double *a, double *b);
void stp(long long a); // for debug
double my_abs(double x);

int main(int argc, char **argv)
{

    if(argc != 3){
        print_usage(argv[0]);
        return 0;
    }
    N_threads = atoll(argv[1]);
    dgb = (N_threads < 0);
    if(dgb){
        printf("Human mode\n");
        N_threads = -N_threads;
    }
    N_points = atoll(argv[2]);
    if(N_points % N_threads){
        print_usage(argv[0]);
        return 0;
    }
    N_points /= N_threads;
    if(dgb)
        printf("%d threads, %lld points per thread\n", N_threads, N_points);

    if((sem_final_res = sem_open("/my_named_posix_semaphore", O_CREAT, 0776, 0)) == SEM_FAILED){
        perror("sem_open (/my_named_posix_semaphore):");
        return EXIT_FAILURE;
    }
    if((sem_thread_launch = sem_open("/my_named_posix_semaphor", O_CREAT, 0776, 0)) == SEM_FAILED){
        perror("sem_open (/my_named_posix_semaphor):");
        return EXIT_FAILURE;
    }

    shm_res = (double*)malloc(N_threads * sizeof(double));
    pthread_t *t_id = (pthread_t*)malloc(sizeof(pthread_t) * N_threads);

    int err, i;
    srand(time(0));
    struct timespec start, stop;

    if(clock_gettime(CLOCK_MONOTONIC,&start) == -1){
        perror("clock gettime");
        return EXIT_FAILURE;
    }
    for(i = 0; i < N_threads; ++i) {
        err = pthread_create(&(t_id[i]), NULL, &compute_pi, NULL);
        if(err){
            printf("can't create thread :[%s]", strerror(err));
        } else {
            if(dgb)
                printf("Thread %d created successfully with t_id=%lu\n", i, (unsigned long)t_id[i]);
        }
        sem_wait(sem_thread_launch);
    }

    if(dgb)
        printf("waiting for threads\n");

    for(i = 0; i < N_threads; ++i)
        sem_wait(sem_final_res);
    if( clock_gettime( CLOCK_MONOTONIC, &stop) == -1 ) {
        perror( "clock gettime" );
        return EXIT_FAILURE;
    }
    double comp_time = (stop.tv_sec - start.tv_sec) + ( stop.tv_nsec - start.tv_nsec) / 1.0e9;

    if(dgb)
        printf("all threads are done\n");

    double pi = 0;
    for(i = 0; i < N_threads; ++i){ // finish summ on the host
        pi += shm_res[i];
    }
    pi /= N_threads;
    if(dgb)
        printf("pi = %lf\ne = %lf\ntime = %lf\n", pi, my_abs(pi * 3 - 1), comp_time);
    else
        //printf("%d %lld %lf\n", N_threads, N_points, comp_time);
        printf("%lf\n", comp_time);

    sem_close(sem_final_res);

    free(t_id);
    free(shm_res);
    return 0;
}

void* compute_pi(void *arg)
{
    int th_ind = th_k;
    ++th_k;
    sem_post(sem_thread_launch);

    pthread_t id = pthread_self();
    long long i = 0;
    long long int sum = 0;
    unsigned int iter = 0;

    struct drand48_data randBuffer;
    srand48_r(time(NULL), &randBuffer);

    double x, y;
    for (i = 0; i < N_points ; ++i) {
        drand48_r(&randBuffer, &x);
        drand48_r(&randBuffer, &y);
        if(y < x * x)
            ++sum;
    }

    shm_res[th_ind] = (double)sum / N_points;
    if(dgb){
        printf("res[%d] = %lf\n", th_ind, shm_res[th_ind]);
    }
    sem_post(sem_final_res);

    return NULL;
}

void my_swap(double *a, double *b)
{
    double c = *a;
    *a = *b;
    *b = c;
}

double my_rnd(void)
{
    return (double)rand() / RAND_MAX;
}

double my_rnd_ab(double a, double b)
{
    if(a > b)
        my_swap(&a, &b);

    return my_rnd() * (b - a) + a;
}

void print_usage(char *exe_name)
{
    printf("usage:\n %s n_threads N_points\n N_points mod n_threads == 0\n", exe_name);
}

void stp(long long a)
{
    printf("%lld\n", a);
    getchar();
}

double my_abs(double x)
{
    return x > 0 ? x : -x;
}
