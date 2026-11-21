#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <stddef.h>
#include <string.h>
#include "kmeans_seq.h"
#include "io.h"
#include "timer.h"

int main(int argc, char* argv[]){

    // Kiểm tra tham số trên console
    if (argc < 5){
        fprintf(stderr, "Chạy theo format sau: %s <input_path> <data_type> <size_label> <run_id> [output_path] [sample_path]", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* data_type = argv[2];
    const char* size_label = argv[3];
    int run_id = atoi(argv[4]);
    const char* output_file = (argc >= 6) ? argv[5] : "results/kmeans_results.csv";
    const char* sample_file = (argc >= 7) ? argv[6] : NULL;

    // Thiêt lập tham số mặc định
    const double tol = 1e-6;
    const int max_iter = 100;
    const int seed = 17;
    const int threads = 1;

    ExecutionTimer timer = {0.0, 0.0, 0.0, 0.0};

    // Đo io time
    double time_io_start = omp_get_wtime();
    size_t n = 0; 
    int d = 0, k = 0;
    double* data = read_kmeans_data(
        input_file,
        &n,
        &d, 
        &k
    );
    timer_record_io(&timer, time_io_start);

    if (n < k){
        fprintf(stderr, "Số lượng điểm dữ liệu (N) không được nhỏ hơn số tâm cụm (K)");
        return 1;
    }

    // Chạy thuật toán tuân tự
    KmeansResult result = run_kmeans_seq(
        data,
        n,
        d,
        k,
        max_iter,
        tol,
        seed
    );
    timer.init_time = result.init_time;
    timer.compute_time = result.compute_time;

    // Tiến hành ghi kết quả tâm cụm và nhãn nếu cần
    if (strcmp(sample_file, "none") != 0 && strcmp(sample_file, "-") != 0){
        double time_write_start = omp_get_wtime();
        write_kmeans_clusters(
            sample_file,
            result.centroids,
            result.labels,
            n,
            d,
            k
        );
        timer.io_time += (omp_get_wtime() - time_write_start);
    }

    // Tính tổng thời gian io, init, tính toán
    timer_finalize(&timer);

    // Ghi kết quả benchmark
    if (strcmp(output_file, "none") != 0 && strcmp(output_file, "-") != 0) {
        if (append_kmeans_benchmark(output_file, "sequential", data_type, size_label,
                                    k, threads, run_id,
                                    timer.io_time, timer.init_time, timer.compute_time,
                                    result.sse, result.iterations) != 0) {
            free_kmeans_result(&result);
            free(data);
            return 1;
        }
    }

    free_kmeans_result(&result);
    free(data);
    return 0;

}