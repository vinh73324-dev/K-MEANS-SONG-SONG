#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <omp.h>

/* ---------------- Do thoi gian ---------------- */
typedef struct timespec timepoint;

/* Lay moc thoi gian (dong ho don dieu, tuong duong steady_clock) */
static timepoint now(void) {
    timepoint t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t;
}

/* Thoi gian (giay) giua hai moc */
static double secs(timepoint a, timepoint b) {
    return (double)(b.tv_sec - a.tv_sec) + (double)(b.tv_nsec - a.tv_nsec) * 1e-9;
}

/* ---------------- MT19937-64 ---------------- */
#define MT_NN 312
#define MT_MM 156
#define MT_MATRIX_A 0xB5026F5AA96619E9ULL
#define MT_UM 0xFFFFFFFF80000000ULL /* 33 bit cao */
#define MT_LM 0x7FFFFFFFULL         /* 31 bit thap */

typedef struct {
    uint64_t mt[MT_NN];
    int mti;
} mt64_t;

static void mt64_seed(mt64_t* s, uint64_t seed) {
    s->mt[0] = seed;
    for (int i = 1; i < MT_NN; ++i)
        s->mt[i] = 6364136223846793005ULL * (s->mt[i - 1] ^ (s->mt[i - 1] >> 62)) + (uint64_t)i;
    s->mti = MT_NN;
}

static uint64_t mt64_next(mt64_t* s) {
    static const uint64_t mag01[2] = {0ULL, MT_MATRIX_A};
    uint64_t x;
    if (s->mti >= MT_NN) {
        int i;
        for (i = 0; i < MT_NN - MT_MM; ++i) {
            x = (s->mt[i] & MT_UM) | (s->mt[i + 1] & MT_LM);
            s->mt[i] = s->mt[i + MT_MM] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];
        }
        for (; i < MT_NN - 1; ++i) {
            x = (s->mt[i] & MT_UM) | (s->mt[i + 1] & MT_LM);
            s->mt[i] = s->mt[i + (MT_MM - MT_NN)] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];
        }
        x = (s->mt[MT_NN - 1] & MT_UM) | (s->mt[0] & MT_LM);
        s->mt[MT_NN - 1] = s->mt[MT_MM - 1] ^ (x >> 1) ^ mag01[(int)(x & 1ULL)];
        s->mti = 0;
    }
    x = s->mt[s->mti++];
    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);
    return x;
}

/* So thuc deu trong [lo, hi) - cach tinh giong std::uniform_real_distribution */
static double mt64_uniform(mt64_t* s, double lo, double hi) {
    double r = (double)mt64_next(s) / 18446744073709551616.0; /* chia 2^64 */
    if (r >= 1.0) r = nextafter(1.0, 0.0);
    return r * (hi - lo) + lo;
}

/* ---------------- Sinh du lieu ---------------- */
/* Cap phat + sinh 2 vecto ngau nhien (cung seed => cung du lieu).
 * Nguoi goi phai free(*a) va free(*b). Tra ve 0 neu thanh cong, -1 neu het bo nho. */
static int generate_vectors(double** a, double** b, long n, unsigned seed) {
    *a = (double*)malloc((size_t)n * sizeof(double));
    *b = (double*)malloc((size_t)n * sizeof(double));
    if (!*a || !*b) {
        free(*a); free(*b);
        *a = *b = NULL;
        return -1;
    }
    mt64_t rng;
    mt64_seed(&rng, (uint64_t)seed);
    for (long i = 0; i < n; ++i) {
        (*a)[i] = mt64_uniform(&rng, -1.0, 1.0);
        (*b)[i] = mt64_uniform(&rng, -1.0, 1.0);
    }
    return 0;
}

/* ---------------- Ghi CSV ---------------- */
/* Ghi them 1 dong vao CSV (tu viet header neu file moi/trong).
 * Tra ve 1 neu thanh cong, 0 neu khong mo duoc file. */
static int append_csv(const char* path, const char* line) {
    FILE* f = fopen(path, "a");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)
        fprintf(f, "mode,n,threads,t_io,t_init,t_compute,t_total,result\n");
    fprintf(f, "%s\n", line);
    fclose(f);
    return 1;
}

/* ---------------- Tinh song song ---------------- */
/* Moi luong giu tong rieng, cuoi cung gop lai => khong race condition */
static double dot_omp(const double* a, const double* b, long n) {
    double s = 0.0;
    long i;
    #pragma omp parallel for schedule(static) reduction(+ : s)
    for (i = 0; i < n; ++i) s += a[i] * b[i];
    return s;
}

/* ---------------- main ---------------- */
int main(int argc, char** argv) {
    if (argc < 3) { fprintf(stderr, "Usage: %s n threads [seed] [csv]\n", argv[0]); return 1; }
    long n = atol(argv[1]);
    int threads = atoi(argv[2]);
    unsigned seed = argc > 3 ? (unsigned)atoi(argv[3]) : 42u;
    const char* csv = argc > 4 ? argv[4] : "results/a1_results.csv";
    omp_set_num_threads(threads);

    double *a = NULL, *b = NULL;
    timepoint t0 = now();
    if (generate_vectors(&a, &b, n, seed) != 0) {
        fprintf(stderr, "Khong du bo nho cho %ld phan tu\n", n);
        return 1;
    }
    timepoint t1 = now();
    double r = dot_omp(a, b, n);
    timepoint t2 = now();

    char line[256];
    snprintf(line, sizeof line, "omp,%ld,%d,0.0,%.9f,%.9f,%.9f,%.12e",
             n, threads, secs(t0, t1), secs(t1, t2), secs(t0, t2), r);
    printf("%s\n", line);
    if (!append_csv(csv, line))
        fprintf(stderr, "Khong ghi duoc %s (thu muc results co ton tai chua?)\n", csv);

    free(a);
    free(b);
    return 0;
}
