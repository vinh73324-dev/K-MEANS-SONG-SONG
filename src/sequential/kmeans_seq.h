#ifndef KMEANS_SEQ_H
#define KMEANS_SEQ_H

#include <stddef.h>

// Đơn giản hóa việc lấy index dữ liệu khi lưu trữ dạng mảng 1D
#define IDX(i, j, D) (((size_t)i * (size_t)D + (size_t)j))

// Cấu trúc lưu trữ kết quả chạy thuật toán Kmeans
typedef struct{
    double* centroids;
    int* labels;
    double sse;
    int iterations;
    double init_time;
    double compute_time;
} KmeansResult;

// Giải phóng bộ nhớ với cấu trúc KmeansResult
void free_kmeans_result(KmeansResult* result);

// Phương thức khởi tạo tâm cụm bằng cách chọn ngẫu nhiên K điểm trong bộ dữ liệu
void init_random_centroids(
    const double* data, 
    double* centroids, 
    size_t n, 
    int d, 
    int k, 
    unsigned int seed
);

// Hàm trả về kết quả bài toán
KmeansResult run_kmeans_seq(
    const double* data,
    size_t n,
    int d,
    int k,
    int max_iter,
    double tol,
    unsigned int seed
);

#endif