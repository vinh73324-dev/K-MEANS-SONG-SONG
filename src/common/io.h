#ifndef IO_H
#define IO_H

#include <stddef.h>

int read_dot_product_data(
    const char* filename,
    double** vec_a,
    double** vec_b,
    size_t* n
);

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

double* read_kmeans_data(
    const char* filename,
    size_t* n,
    int* d,
    int* k
);

int write_kmeans_clusters(
    const char* filename,
    const double* centroids,
    const int* labels,
    size_t n,
    int d,
    int k
);

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
