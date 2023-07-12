/********************************************************
clock resolution and current time (since epoch)

  clang -o clock_check clock_check.c

saleem, Jan2022, Jan2021
*********************************************************/

#include <stdio.h>
#include <time.h>

int printTimeUnits();
int printTime();

int printTimeUnits()
{
  struct timespec t;

  if (clock_gettime(CLOCK_REALTIME, &t) == 0) {
    if (t.tv_sec > 0) printf(" %ld s", t.tv_sec);
    if (t.tv_nsec > 0) printf(" %ld ns", t.tv_nsec);
  }

  return 0;
}

int printTime()
{
  struct timespec t;

  if (clock_gettime(CLOCK_REALTIME, &t) == 0) {
    if (t.tv_sec > 0) printf("%ld.", t.tv_sec);
    if (t.tv_nsec > 0) printf("%ld ", t.tv_nsec);
  }

  return 0;
}
