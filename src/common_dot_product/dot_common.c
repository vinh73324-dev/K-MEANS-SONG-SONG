#define _POSIX_C_SOURCE 199309L /* de clock_gettime co san khi dung -std=c99 tren Linux */

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ---- Ham do thoi gian (giay), tren Linux ---- */
static double now_sec(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0.0;
    }
    return (double)ts.tv_sec + 1e-9 * (double)ts.tv_nsec;
}

/* ---- Bo sinh so ngau nhien xorshift64* (cung seed => cung du lieu) ---- */
static uint64_t rng_state = 1ULL;

static void rng_seed(unsigned int seed) {
    rng_state = 88172645463325252ULL ^ ((uint64_t)seed * 2654435761ULL);
    if (rng_state == 0ULL) {
        rng_state = 1ULL;
    }
}

static double rng_uniform(void) { /* gia tri trong [-1.0, 1.0) */
    rng_state ^= rng_state >> 12;
    rng_state ^= rng_state << 25;
    rng_state ^= rng_state >> 27;
    uint64_t x = rng_state * 2685821657736338717ULL;
    return ((double)(x >> 11) / 0x1p53) * 2.0 - 1.0;
}

/* ---- Cap phat + sinh 2 vecto. Tra ve 0 neu OK, -1 neu loi / het bo nho ---- */
static int generate_vectors(double** a, double** b, long n, unsigned int seed) {
    long i;
    if (!a || !b || n <= 0) {
        return -1;
    }
    if ((size_t)n > SIZE_MAX / sizeof(double)) {
        return -1;
    }

    *a = (double*)malloc((size_t)n * sizeof(double));
    *b = (double*)malloc((size_t)n * sizeof(double));
    if (!*a || !*b) {
        free(*a);
        free(*b);
        *a = NULL;
        *b = NULL;
        return -1;
    }

    rng_seed(seed);
    for (i = 0; i < n; ++i) {
        (*a)[i] = rng_uniform();
        (*b)[i] = rng_uniform();
    }
    return 0;
}

/* ---- Ghi them 1 dong vao CSV (tu viet header neu file moi). Tra ve 0 neu OK ---- */
static int append_csv(const char* path, const char* line) {
    FILE* f;
    long pos;
    if (!path || !line) {
        return -1;
    }

    f = fopen(path, "a");
    if (!f) {
        return -1;
    }

    pos = -1;
    if (fseek(f, 0, SEEK_END) == 0) {
        pos = ftell(f);
    }

    if (pos == 0) {
        if (fprintf(f, "mode,n,threads,t_io,t_init,t_compute,t_total,result\n") < 0) {
            fclose(f);
            return -1;
        }
    }

    if (fprintf(f, "%s\n", line) < 0 || ferror(f)) {
        fclose(f);
        return -1;
    }

    if (fclose(f) != 0) {
        return -1;
    }
    return 0;
}

int main(int argc, char** argv) {
    char* endptr = NULL;
    long n;
    unsigned int seed;
    double *a = NULL, *b = NULL;
    double t0, t1;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <n> [seed]\n", argv[0]);
        return 1;
    }

    errno = 0;
    n = strtol(argv[1], &endptr, 10);
    if (errno != 0 || endptr == argv[1] || *endptr != '\0' || n <= 0) {
        fprintf(stderr, "Gia tri n khong hop le: %s\n", argv[1]);
        return 1;
    }

    seed = 42u;
    if (argc > 2) {
        long s;
        errno = 0;
        endptr = NULL;
        s = strtol(argv[2], &endptr, 10);
        if (errno != 0 || endptr == argv[2] || *endptr != '\0' || s < 0) {
            fprintf(stderr, "Gia tri seed khong hop le: %s\n", argv[2]);
            return 1;
        }
        seed = (unsigned int)s;
    }

    t0 = now_sec();
    if (generate_vectors(&a, &b, n, seed) != 0) {
        fprintf(stderr, "Khong the cap phat bo nho cho n = %ld\n", n);
        return 2;
    }
    t1 = now_sec();

    printf("Sinh %ld phan tu x2 vecto mat %.6f s\n", n, t1 - t0);

    free(a);
    free(b);
    return 0;
}
