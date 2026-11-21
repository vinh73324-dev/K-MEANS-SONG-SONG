#include <stdio.h>
#include <omp.h>
#include "timer.h"

// Hàm khởi tạo bộ đếm thời gian (reset toàn bộ giá trị về 0.0)
void timer_init(ExecutionTimer* timer){
    if(!timer){
        fprintf(stderr, "Loi: Bo dem thoi gian khong duoc null\n");
        return;
    }
    timer->io_time = 0.0;
    timer->init_time = 0.0;
    timer->compute_time = 0.0;
    timer->total_time = 0.0;
}

// Lấy thời gian thực tại
double timer_now(void){
    return omp_get_wtime();
}

// Ghi nhận thời gian IO (từ mốc start đến thời điểm gọi hàm)
void timer_record_io(ExecutionTimer* timer, double start_time){
    double end_time = omp_get_wtime();
    timer->io_time = end_time - start_time;
}

// Ghi nhận thời gian khởi tạo (từ mốc start đến thời điểm gọi hàm)
void timer_record_init(ExecutionTimer* timer, double start_time){
    double end_time = omp_get_wtime();
    timer->init_time = end_time - start_time;
}

// Ghi nhận thời gian tính toán (từ mốc start đến thời điểm gọi hàm)
void timer_record_compute(ExecutionTimer* timer, double start_time){
    double end_time = omp_get_wtime();
    timer->compute_time = end_time - start_time;
}

// Tính tổng thời gian của chương trình
void timer_finalize(ExecutionTimer* timer){
    timer->total_time = timer->io_time + timer->init_time + timer->compute_time;
}