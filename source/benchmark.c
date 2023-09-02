#include "benchmark.h"

#include "stdio.h"
#include "time.h"

static clock_t startTime;

void start_timer()
{
    startTime = clock();
}

void end_timer(const char *label)
{
    double timeTaken = (double)(clock() - startTime) / CLOCKS_PER_SEC * 1000.0;

    printf("%s: %.0fms\n", label, timeTaken);
}