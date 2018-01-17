#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <inttypes.h>
#include <sys/syscall.h>

#define SAMPLES 30
#define SAMPLES_PER_SEC 10
#define SAMPLE_LOOPS 40

/**
Source : https://mlichvar.fedorapeople.org/tmp/tstsc.c
Description: Test that using  TSC and clock_gettime to
 exercise the clock a little bit then disturbs the tick
 adjusting code.
*/


struct sample {
    uint64_t tsc;
    uint64_t ts;
    int delay;
};

static uint64_t rdtsc() {
#if 1
#if defined(__powerpc64__) || defined(__ppc64__)
        uint32_t tbu, tbl, tmp;
        __asm__ volatile(\
         "0:\n"
         "mftbu %0\n"
         "mftbl %1\n"
         "mftbu %2\n"
         "cmpw %0, %2\n"
         "bne- 0b"
          : "=r" (tbu), "=r"(tbl), "=r" (tmp));
        return (((uint64_t) tbu << 32) | tbl);
#else
    uint32_t a, d;
    __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d));
    return (uint64_t)d << 32 | a;
#endif
#else
    return __builtin_ia32_rdtsc();
#endif
}

static void get_sample(struct sample *sample) {
    int i, delay, mindelay;
    struct timespec ts = {0, 0}, ts1;
    uint64_t tsc = 0, tsc1, tsc2;

    for (i = 0, mindelay = INT_MAX; i < SAMPLE_LOOPS; i++) {
        tsc1 = rdtsc();
#if 1
        clock_gettime(CLOCK_REALTIME, &ts1);
#else
        syscall(__NR_clock_gettime, CLOCK_REALTIME, &ts1);
#endif
        tsc2 = rdtsc();
        delay = tsc2 - tsc1;
        if (delay <= mindelay) {
            ts = ts1;
            tsc = (tsc1 + tsc2) / 2;
            mindelay = delay;
        }
    }
    sample->tsc = tsc;
    sample->ts = (uint64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
    sample->delay = mindelay;
}

/* taken/modified from chrony/regress.c */
void
regress
(double *x,                     /* independent variable */
 double *y,                     /* measured data */
 int n,                         /* number of data points */

 /* And now the results */

 double *b0,                    /* estimated y axis intercept */
 double *b1,                    /* estimated slope */
 double *s2

 /* Could add correlation stuff later if required */
)
{
  double P, Q, U, V, W;
  double diff;
  double u, ui;
  int i;

  assert(n >= 3);

  W = U = 0;
  for (i=0; i<n; i++) {
    U += x[i];
    W += 1.0;
  }

  u = U / W;

  /* Calculate statistics from data */
  P = Q = V = 0.0;
  for (i=0; i<n; i++) {
    ui = x[i] - u;
    P += y[i];
    Q += y[i] * ui;
    V += ui   * ui;
  }

  *b1 = Q / V;
  *b0 = (P / W) - (*b1) * u;

  *s2 = 0.0;
  for (i=0; i<n; i++) {
    diff = y[i] - *b0 - *b1*x[i];
    *s2 += diff*diff;
  }

  *s2 /= (double)(n-2);
}

int main() {
    struct sample samples[SAMPLES];
    double x[SAMPLES], y[SAMPLES], slope, intercept, offset, variance, max_offset;
    int i;

#if 1
    usleep(100000);
#endif
    for (i = 0; i < SAMPLES; i++) {
        get_sample(samples + i);
#if 0
        usleep(rand() % 2000000 / SAMPLES_PER_SEC);
#else
        usleep(1000000 / SAMPLES_PER_SEC);
#endif
    }

    for (i = 0; i < SAMPLES; i++) {
        x[i] = samples[i].tsc - samples[0].tsc;
        y[i] = samples[i].ts - samples[0].ts;
    }

    regress(x, y, SAMPLES, &intercept, &slope, &variance);
    max_offset = 0.0;

    for (i = 0; i < SAMPLES; i++) {
        offset = x[i] * slope + intercept - y[i];
        if (fabs(offset) > max_offset)
            max_offset = fabs(offset);
        printf("%5d %5d %9.1f\n", i, samples[i].delay, offset);
    }

    printf("n: %d, slope: %.2f (%.2f GHz), dev: %.1f ns, max: %.1f ns\n", SAMPLES, slope, 1.0 / slope, sqrt(variance), max_offset);
    return 0;
}
