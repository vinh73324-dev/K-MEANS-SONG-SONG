#ifndef IO_H
#define IO_H

#include <stddef.h>

// Hàm đọc data TVH
int read_dot_product_data(
    const char* filename,
    double** vec_a,
    double** vec_b,
    size_t* n
);

// Hàm bổ sung 1 bản ghi vào kết quả của chương trình tính TVH
int append_dot_product_benchmark(
    const char* filename,
    const char* algo,
    const char* size_label,
    size_t n,
    int threads,
    int run_id,
    double init_time,
    double io_time,
    double compute_time,
    double dot_result
);

// Hàm đọc data Kmeans
double* read_kmeans_data(
    const char* filename,
    size_t* n,
    int* d,
    int* k
);

// Hàm ghi các tâm cụm cùng nhãn của các data sau khi thuật toán Kmeans dừng
int write_kmeans_clusters(
    const char* filename,
    const double* centroids,
    const int* labels,
    size_t n,
    int d,
    int k
);

// Hàm bổ sung 1 bản ghi vào kết quả của chương trình Kmeans
int append_kmeans_benchmark(
    const char* filename,
    const char* algo,
    const char* data_type,
    const char* size_label,
    int k, 
    int threads, 
    int run_id,
    double io_time, 
    double init_time,
    double compute_time,
    double sse, 
    int iterations
);

#endif
