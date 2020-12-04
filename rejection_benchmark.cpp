#include <err.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdbool.h>

#include <gem5/m5ops.h>

#define PAGE    4096
#define BLOCK   64

char * data = NULL;
int size = 0;
int num_threads = 0;

struct result_s
{
    int skiped;
    int rejected;
    int processed;
};

void reset_stats()
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x40;");
}

void dump_stats()
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x41;");
}

void dump_reset_stats()
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x42;");
}

void gem5(char * pointer)
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x55;");
}

void gem5_clean(char * pointer)
{
    __asm__ __volatile__ (".byte 0x0F, 0x04; .word 0x56;");
}

void *thread(void *args) 
{
    struct result_s *result = (struct result_s *)args;
    result->skiped = 0;
    result->rejected = 0;
    result->processed = 0;

    for(int i = 0; i < size; i++) {
        if ( i % 0x100 == 0 )
            result->skiped++;
        else if ( *(data + i) == 0 )
            result->rejected++;
        else
            result->processed++;
    }
}

int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating Y. */
    if (x->tv_usec < y->tv_usec)
    {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000)
    {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       `tv_usec' is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

int
main(int argc, char *argv[])
{
    struct timeval timeval1, timeval2, timeval_result;

    if(argc <= 2) {
        printf("%s <size of memory> <number of threads>\n", argv[0]);
        return -1;
    }

    size = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    bool enable_rejection = argc == 3;

    pthread_t threads[num_threads];
    struct result_s results[num_threads];

    data = (char*) memalign(PAGE, size);

    for(int i = 0; i < size; i++) {
        *(data + i) = i;
    }

    if ( enable_rejection )
    {
        for(int i = 0; i < size; i += BLOCK) {
            gem5(data + i);
        }
    }

    gettimeofday(&timeval1, NULL);
    reset_stats();

    for(int i = 0; i < num_threads; i++) {
        pthread_create(threads + i, NULL, thread, (void *)(results + i));
    }

    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    dump_stats();
    gettimeofday(&timeval2, NULL);

    for(int i = 0; i < num_threads; i++) {
        printf("thread %d - skiped %d rejected %d processed %d\n", i, results[i].skiped, results[i].rejected, results[i].processed);
    }

    timeval_subtract(&timeval_result, &timeval2, &timeval1);

    printf("runtime: %lu.%lu seconds.\n", timeval_result.tv_sec, timeval_result.tv_usec);

    if ( enable_rejection )
        gem5_clean(data);

    return 0;
}
