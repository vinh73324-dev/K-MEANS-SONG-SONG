#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#include "kmeans_seq.h"

void init_random_centroids(
    const double* data,
    double* centroids,
    size_t n,
    int d,
    int k,
    unsigned int seed
){
    srand(seed);

    // Khởi tạo mảng hệ số
    size_t* indices = (size_t*)malloc(n * sizeof(size_t));
    for (size_t i = 0; i < n; i++){
        indices[i] = i;
    }

    // Chọn tâm cụm ngẫu nhiên trên dữ liệu đã có, đảm bảo không trùng lặp
    for (int c = 0; c < k; c++){
        int rand_idx = c + rand() % (n - c);

        // Đổi vị trí index
        size_t temp = indices[c];
        indices[c] = indices[rand_idx];
        indices[rand_idx] = temp;

        size_t chosen_point = indices[c];

        // Sao chép tọa độ của điểm được chọn vào tâm thứ c
        memcpy(
            &centroids[(size_t)c * d],
            &data[chosen_point * d],
            d * sizeof(double)
        );
    }

    free(indices);
}

KmeansResult run_kmeans_seq(
    const double* data,
    size_t n,
    int d, 
    int k,
    int max_iter,
    double tol,
    unsigned int seed
){
    
    // === ĐO INIT_TIME ===
    double init_time_start = omp_get_wtime();

    KmeansResult result = {NULL, NULL, 0.0, 0, 0.0, 0.0};
    result.centroids = (double*)malloc(k * d * sizeof(double));
    result.labels = (int*)calloc(n, sizeof(int));

    
    int* counts = (int*)malloc(k * sizeof(int)); // Mảng đếm số điểm thuộc cụm thứ i
    double* new_centroids = (double*)malloc(k * d * sizeof(double)); // Mảng lưu trữ tâm cụm mới

    init_random_centroids(
        data,
        result.centroids,
        n,
        d,
        k,
        seed
    );

    int iter = 0;
    double sse = 0.0; // Biến lưu trữ tổng bình phương khoảng cách
    
    result.init_time = omp_get_wtime() - init_time_start;

    // === ĐO COMPUTE_TIME ===
    double compute_time_start = omp_get_wtime();

    for (iter = 0; iter < max_iter; iter++){
        memset(new_centroids, 0, sizeof(new_centroids));
        memset(counts, 0, sizeof(counts));
        sse = 0.0;

        // Gán điểm vào cụm gần nhất
        for (size_t i = 0; i < n; i++){
            double min_dist = INFINITY;
            int best_cluster = 0;
            for (int c = 0; c < k; c++){
                double curr_dist = 0;
                for (int j = 0; j < d; j++){
                    double diff = (data[IDX(i, j, d)] - result.centroids[IDX(c, j, d)]);
                    curr_dist += diff * diff;
                }
                if (curr_dist < min_dist){
                    min_dist = curr_dist;
                    best_cluster = c;
                }
            }
            result.labels[i] = best_cluster;
            counts[best_cluster] += 1;
            sse += min_dist;
            
            // Cộng dồn tọa độ chuẩn bị tìm tâm mới
            for (int j = 0; j < d; j++){
                new_centroids[IDX(best_cluster, j, d)] += data[IDX(i, j, d)];
            }
        }

        // Cập nhật tâm cụm
        double max_sq_cent_diff = 0.0;
        for (int c = 0; c < k; c++){
            double sq_cent_diff = 0.0;
            if(counts[c] > 0){
                for (int j = 0; j < d; j++){
                    double updated_dim_val = new_centroids[IDX(c, j, d)] / counts[c];
                    double diff = updated_dim_val - result.centroids[IDX(c, j, d)];
                    sq_cent_diff += diff * diff;
                    result.centroids[IDX(c, j, d)] = updated_dim_val;
                }
                if (sq_cent_diff > max_sq_cent_diff){
                    max_sq_cent_diff = sq_cent_diff;
                }
            }
        }

        // Điều kiện dừng
        if (max_sq_cent_diff < tol * tol){
            iter++;
            break;
        }

    }
    result.compute_time = omp_get_wtime() - compute_time_start;

    result.iterations = iter;
    result.sse = sse;

    free(counts);
    free(new_centroids);

    return result;
}