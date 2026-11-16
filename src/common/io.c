#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "io.h"

int read_dot_product_data(
    const char* filename,
    double** vec_a,
    double** vec_b,
    size_t* n
){
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        fprintf(stderr, "Loi: Khong mo duoc file %s\n", filename);
        return -1;
    }
    
    // Đọc số phần tử n
    if (fscanf(file, "%zu", n) != 1){
        fprintf(stderr, "Loi: Sai dinh dang header trong %s\n", filename);
        fclose(file);
        return -1;
    }

    // Cấp phát bộ nhớ động
    *vec_a = malloc(*n * sizeof(double));
    *vec_b = malloc(*n * sizeof(double));

    if (!(*vec_a) || !(*vec_b)){
        fprintf(stderr, "Loi: Khong du bo nho cap phat cho %zu phan tu\n", *n);
        free(*vec_a);
        free(*vec_b);
        *vec_a = NULL;
        *vec_b = NULL;
        fclose(file);
        return -1;
    }

    // Đọc vector A
    for (size_t i = 0; i < *n; i++){
        if (fscanf(file, "%lf,", &(*vec_a)[i]) != 1) goto read_error;
    }

    // Đọc vector B
    for (size_t i = 0; i < *n; i++){
        if (fscanf(file, "%lf,", &(*vec_b)[i]) != 1) goto read_error;
    }
    fclose(file);
    return 0;

    // Nhãn lỗi khi thao tác đọc vector không thành công
read_error:
    fprintf(stderr, "Loi: Du lieu doc bi sai\n");
    free(*vec_a);
    free(*vec_b);
    *vec_a = NULL;
    *vec_b = NULL;
    fclose(file);
    return -1;
}

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
){
    FILE *file = fopen(filename, "a");
    if (file == NULL){
        fprintf(stderr, "Loi: Khong the mo file\n");
        return -1;
    }

    double total_time = init_time + io_time + compute_time;

    fprintf(file, "%s,%s,%zu,%d,%d,%.6f,%.6f,%.6f,%.6f,%.6f\n",
            algo, size_label, n, threads, run_id,
            init_time, io_time, compute_time, total_time, dot_result);

    fclose(file);
    return 0;
}

double* read_kmeans_data(
    const char* filename,
    size_t* n,
    int* d,
    int* k
){
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        fprintf(stderr, "Loi: Khong mo duoc file %s\n", filename);
        return NULL;
    }

    // Đọc N, D, K
    if (fscanf(file, "%zu,%d,%d", n, d, k) != 3){
        fprintf(stderr, "Loi: Sai dinh dang header trong %s\n", filename);
        fclose(file);
        return NULL;
    }
    
    // Lưu trữ dữ liệu bằng cấp phát động mảng 1D
    size_t totals = *n * (size_t)(*d);
    double* result = malloc(totals * sizeof(double));

    if (!result){
        fprintf(stderr, "Loi: Khong du bo nho cap phat\n");
        fclose(file);
        return NULL;
    }

    for (size_t i = 0; i < totals; i++){
        if (fscanf(file, "%lf,", &result[i]) != 1){
            fprintf(stderr, "Loi: Du lieu doc bi sai\n");
            free(result);
            fclose(file);
            return NULL;   
        }
    }

    fclose(file);
    return result;
}

int write_kmeans_clusters(
    const char* filename,
    const double* centroids,
    const int* labels,
    size_t n,
    int d,
    int k 
){
    FILE *file = fopen(filename, "w");
    if (file == NULL){
        fprintf(stderr, "Loi: Khong the mo file\n");
        return -1;
    }

    // Lưu trữ các tâm cụm cuối bước lặp
    fprintf(file, "# CENTROID (K = %d, D = %d)\n", k, d);
    for (int c = 0; c < k; c++){
        fprintf(file, "%d", c);
        for (int i = 0; i < d; i++){
            fprintf(file, ",%lf", centroids[(size_t)c * d + i]);
        }
        fprintf(file, "\n");
    }

    // Lưu trữ nhãn của dữ liệu cuối bước lặp
    fprintf(file, "# LABELS (N = %zu)\n", n);
    for (size_t i = 0; i < n; i++){
        fprintf(file, "%zu,%d\n", i, labels[i]);
    }

    fclose(file);
    return 0;
}

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
){
    FILE *file = fopen(filename, "a");
    if (file == NULL){
        fprintf(stderr, "Loi: Khong the mo file\n");
        return -1;
    }

    double total_time = io_time + init_time + compute_time;

    fprintf(file, "%s,%s,%s,%d,%d,%d,%.6f,%.6f,%.6f,%.6f,%.4f,%d\n",
            algo, data_type, size_label, k, threads, run_id,
            io_time, init_time, compute_time, total_time, sse, iterations);

    fclose(file);
    return 0;
}
