import os
import numpy as np
from sklearn.datasets import make_blobs

def generate_kmeans_data(
    file_name,
    n_samples,
    n_features,
    n_clusters,
    data_type = "blobs",
    low = 0.0,
    high = 100.0,
    seed = 18   
):
    """
    Hàm sinh dữ liệu K-means gồm 3 loại (blobs, uniform và varied_std) và ghi ra file CSV.
    - Dòng 1: chứa N (số điểm) và D (số chiều)
    - N dòng tiếp theo: tọa độ D chiều của từng điểm, cách nhau bằng dấu phẩy
    """
    print(f"Đang sinh dữ liệu KMEANS dạng {data_type.upper()}: N = {n_samples}, D = {n_features}, K = {n_clusters} -> {file_name}")
    
    if data_type == "blobs":
        # Loại 1: Dữ liệu phân cụm đồng nhất
        X = make_blobs(
            n_samples = n_samples,
            n_features = n_features,
            centers = n_clusters,
            center_box = (low, high),
            cluster_std = 1.5,
            random_state = seed
        )
        data = X[0]
    elif data_type == "uniform":
        # Loai 2: Dữ liệu phân phối đều ngẫu nhiên
        np.random.seed(seed)
        data = np.random.uniform(low, high, size = (n_samples, n_features))
    elif data_type == "varied_std":
        # Loai 3: Dữ liệu phân cụm không đồng nhất
        stds = [0.5 * (i+1) for i in range(n_clusters)]
        X = make_blobs(
            n_samples = n_samples,
            n_features = n_features,
            centers = n_clusters,
            cluster_std = stds,
            random_state = seed
        )
        data = X[0]
    else:
        raise ValueError(f"Không thể sinh loại dữ liệu {data_type}")
    
    # Thực hiện tạo file (nếu chưa tạo) và ghi dữ liệu sinh vào file
    os.makedirs(os.path.dirname(file_name), exist_ok = True)
    with open(file_name, "w", encoding = "utf-8") as f:
        f.write(f"{n_samples},{n_features}\n")
        np.savetxt(f, data, delimiter = ",", fmt = "%.6f")
    
    print(f"Dữ liệu KMEANS dạng {data_type} đã được lưu vào {file_name}")

def generate_dot_product_data(
    file_name, 
    size, 
    low = 0.0, 
    high = 100.0, 
    seed = 17
):
    """
    Hàm sinh dữ liệu cho bài toán tích vô hướng 2 vector:
    - Sinh 2 vector A, B
    - Ghi kết quả vào file CSV
    """
    print(f"Đang sinh dữ liệu TVH Vector {size} thành phần -> {file_name}")
    # Thực hiện sinh 2 vector A, B
    np.random.seed(seed)
    A = np.random.uniform(low, high, size)
    B = np.random.uniform(low, high, size)

    # Thực hiện tạo file (nếu chưa có file) và tiến hành ghi dữ liệu
    os.makedirs(os.path.dirname(file_name), exist_ok = True)
    with open(file_name, "w", encoding = "utf-8") as f:
        np.savetxt(f, [A], delimiter = ",", fmt = "%.6f")
        np.savetxt(f, [B], delimiter = ",", fmt = "%.6f")

    print(f"Dữ liệu TVH đã lưu vào {file_name}")
    
if __name__ == "__main__":
    pass
