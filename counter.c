/* concurrentcounter.c
 * Autor: Gina Kokoska
 * Datum: 23.11.2021
 * /

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <sys/sysinfo.h>

#define NUMLOOPS 10000

struct timespec start, end, result;

/*stuct to define a counter*/
typedef struct __counter_t {
        int value;
        pthread_mutex_t lock;
} counter_t;

/*function initializing counter with 0 and lock for counter*/
void init(counter_t *c) {
        c->value = 0;
        pthread_mutex_init(&c->lock, NULL);
}

/*function to increment counter lock before changing value and unlock after done*/
void increment(counter_t *c) {
        pthread_mutex_lock(&c->lock);
        c->value++;
        pthread_mutex_unlock(&c->lock);
}

/*functin to decrement counter lock */
void decrement(counter_t *c) {
        pthread_mutex_lock(&c->lock);
        c->value--;
        pthread_mutex_unlock(&c->lock);
}

/*function to return counter value*/
int get(counter_t *c) {
        pthread_mutex_lock(&c->lock);
        int rc = c->value;
        pthread_mutex_unlock(&c->lock);
        return rc;
}

/*function pointer for threads to increment counter */
void *mythread_i(void *arg) {
        counter_t *c = (counter_t *) arg;
        for (int i = 0; i < NUMLOOPS; i++) {
                increment(c);
        }
        return NULL;
}

/*function pointer for threads to decrement counter*/
void *mythread_d(void *arg) {
        counter_t *c = (counter_t *) arg;
        for (int i = 0; i < NUMLOOPS; i++) {
                decrement(c);
        }
        return NULL;
}

/*function pointer for threadmeasurement*/
void *donothing(void *arg) {
        return NULL;
}

/*function to calculate elapsed timep*/
typedef uint64_t(*timeconvert)(struct timespec, struct timespec, struct timespec);

uint64_t convert(struct timespec start, struct timespec end, struct timespec result){
        result.tv_sec = end.tv_sec - start.tv_sec;
        result.tv_nsec = end.tv_nsec - start.tv_nsec;
        uint64_t res = (result.tv_sec * 1000000000) + result.tv_nsec;
        return(res);
}

/*function to measure raw loop*/
typedef uint64_t(*getlooptime)(struct timespec, struct timespec, struct timespec);

uint64_t loopfunc(struct timespec loop_s, struct timespec loop_e, struct timespec loop_r) {
        clock_gettime(CLOCK_MONOTONIC, &loop_s);
        for (int i = 0; i < NUMLOOPS; i++) {};
        clock_gettime(CLOCK_MONOTONIC, &loop_e);
        timeconvert loop_c = &convert;
        return(loop_c(loop_s, loop_e, loop_r));
        }

typedef uint64_t(*getthreadtime)(struct timespec, struct timespec, struct timespec);

uint64_t threadfunc(struct timespec thread_s, struct timespec thread_e, struct timespec thread_r) {
        pthread_t n;
        clock_gettime(CLOCK_MONOTONIC, &thread_s);
        int rn = pthread_create(&n, NULL, donothing, NULL);
        assert(rn == 0);
        pthread_join(n, NULL);
        clock_gettime(CLOCK_MONOTONIC, &thread_e);
        timeconvert thread_c = &convert;
        return(thread_c(thread_s, thread_e, thread_r));
}
                                                                                                                                                                                          int main(int argc, char** argv) {
int main(int argc, char** argv) {

        int cpu = get_nprocs();
        pthread_t x[cpu];
        int rc[cpu];
        uint64_t  t, rawloop_t, rawthread_t;
        counter_t y;

        printf("Number of CPUS: %d\n", cpu);

        init(&y);
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 1 ; i < cpu+1; i++) {
                rc[i] = pthread_create(&x[i], NULL, mythread_i, &y);
                assert(rc[i] == 0);
        }
        for (int i = 1; i < cpu+1; i++) {
                pthread_join(x[i], NULL);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        timeconvert time = &convert;
        t = time(start, end, result);
        getlooptime rawloop = &loopfunc;
        rawloop_t = rawloop(loop_s, loop_e, loop_r);
        getthreadtime rawthread = &threadfunc;
        rawthread_t = rawthread(thread_s, thread_e, thread_r);

        if (t < (rawloop_t + rawthread_t)) {
                t = t;
        } else {
                t = t - (rawloop_t * cpu) - (rawthread_t * cpu);
        }
        printf("Time for accessing critical section using [%d] threads in ns:\t%lu\n",cpu, t);
        printf("Value of counter %d\n", get(&y));

return 0;

}




                                                                                                                                                                                    

