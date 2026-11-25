#include <cstdio>
#include <cstdlib>
#include <vector>
#include <random>
#include <chrono>

typedef std::chrono::steady_clock Clock;
static double secs(Clock::time_point a, Clock::time_point b) { return std::chrono::duration<double>(b - a).count(); }

static void generate_vectors(std::vector<double>& a, std::vector<double>& b, long n, unsigned seed) {
    a.resize(n); b.resize(n);
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> U(-1.0, 1.0);
    for (long i = 0; i < n; ++i) { a[i] = U(rng); b[i] = U(rng); }
}

static bool append_csv(const char* path, const char* line) {
    FILE* f = fopen(path, "a");
    if (!f) return false;
    if (ftell(f) == 0) fprintf(f, "mode,n,threads,t_io,t_init,t_compute,t_total,result\n");
    fprintf(f, "%s\n", line);
    fclose(f);
    return true;
}

// Ham tinh tuan tu
static double dot_seq(const double* a, const double* b, long n) {
    double s = 0.0;
    for (long i = 0; i < n; ++i) s += a[i] * b[i];
    return s;
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s n [seed] [csv]\n", argv[0]); return 1; }
    long n = atol(argv[1]);
    unsigned seed = argc > 2 ? (unsigned)atoi(argv[2]) : 42u;
    const char* csv = argc > 3 ? argv[3] : "results/a1_results.csv";

    std::vector<double> a, b;
    Clock::time_point t0 = Clock::now();
    generate_vectors(a, b, n, seed);
    Clock::time_point t1 = Clock::now();
    double r = dot_seq(a.data(), b.data(), n);
    Clock::time_point t2 = Clock::now();

    char line[256];
    snprintf(line, sizeof line, "seq,%ld,1,0.0,%.9f,%.9f,%.9f,%.12e", n, secs(t0, t1), secs(t1, t2), secs(t0, t2), r);
    printf("%s\n", line);
    if (!append_csv(csv, line)) fprintf(stderr, "Khong ghi duoc %s (thu muc results co ton tai chua?)\n", csv);
    return 0;
}
