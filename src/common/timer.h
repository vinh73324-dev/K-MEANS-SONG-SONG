#ifndef TIMER_H
#define TIMER_H

struct{
    double io_time,
    double init_time,
    double compute_time,
    double total_time
} ExecutionTimer;

// Hàm khởi tạo bộ đếm thời gian (reset toàn bộ giá trị về 0.0)
void timer_init(ExecutionTimer* timer);

// Lấy thời gian thực tại
double timer_now(void);

// Ghi nhận thời gian IO (từ mốc start đến thời điểm gọi hàm)
double timer_record_io(ExecutionTimer* timer, double start_time);

// Ghi nhận thời gian khởi tạo (từ mốc start đến thời điểm gọi hàm)
double timer_record_init(ExecutionTimer* timer, double start_time);

// Ghi nhận thời gian tính toán (từ mốc start đến thời điểm gọi hàm)
double timer_record_comput(ExecutionTimer* timer, double start_time);

// Tính tổng thời gian của chương trình
double timer_record_total(ExecutionTimer* timer);
#endif