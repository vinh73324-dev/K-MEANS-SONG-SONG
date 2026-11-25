#include <cstdio>
#include <cstdlib>
#include <vector>
#include <random>
#include <chrono>

typedef std::chrono::steady_clock Clock;

// Ham do thoi gian (giay) giua hai moc
double secs(Clock::time_point a, Clock::time_point b) {
    return std::chrono::duration<double>(b - a).count();
}

// Cap phat + sinh 2 vecto ngau nhien (cung seed => cung du lieu)
void generate_vectors(std::vector<double>& a, std::vector<double>& b, long n, unsigned seed) {
    a.resize(n); b.resize(n);
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> U(-1.0, 1.0);
    for (long i = 0; i < n; ++i) { a[i] = U(rng); b[i] = U(rng); }
}

// Ghi them 1 dong vao CSV (tu viet header neu file moi/trong). Tra ve false neu khong mo duoc file.
bool append_csv(const char* path, const char* line) {
    FILE* f = fopen(path, "a");
    if (!f) return false;
    if (ftell(f) == 0)
        fprintf(f, "mode,n,threads,t_io,t_init,t_compute,t_total,result\n");
    fprintf(f, "%s\n", line);
    fclose(f);
    return true;
}

int main(int argc, char** argv) {
    if (argc < 2) { fprintf(stderr, "Usage: %s n [seed]\n", argv[0]); return 1; }
    long n = atol(argv[1]);
    unsigned seed = argc > 2 ? (unsigned)atoi(argv[2]) : 42u;
    std::vector<double> a, b;
    Clock::time_point t0 = Clock::now();
    generate_vectors(a, b, n, seed);
    Clock::time_point t1 = Clock::now();
    printf("Sinh %ld phan tu x2 vecto mat %.6f s\n", n, secs(t0, t1));
    return 0;
}
