import sys
import csv
import json
import matplotlib.pyplot as plt
from collections import defaultdict
from tabulate import tabulate

# 设置字体，避免中文显示问题
plt.rcParams.update({
    'font.sans-serif': ['DejaVu Sans'],
    'axes.unicode_minus': False,
    'figure.dpi': 150
})

def load_calibration(calib_file):
    """加载校准参数文件"""
    try:
        with open(calib_file) as f:
            return json.load(f)
    except:
        print(f"Warning: No calibration file found at {calib_file}, using defaults")
        return {
            'accel_bias': [0.0, 0.0, 0.0],
            'accel_scale': [1.0, 1.0, 1.0],
            'gyro_bias': [0.0, 0.0, 0.0]
        }

def process_imu_data(filename, calib_file='calibration.json'):
    calib = load_calibration(calib_file)

    time_stamps = []
    accel = [[] for _ in range(3)]
    gyro = [[] for _ in range(3)]
    temperature = []
    accel_filt = [[] for _ in range(3)]
    gyro_filt = [[] for _ in range(3)]
    temp_filt = []

    quat = [[] for _ in range(4)]  # W, X, Y, Z
    euler = [[] for _ in range(3)] # Roll, Pitch, Yaw

    valid_line_count = 0

    with open(filename, 'r') as f:
        csv_reader = csv.reader(f)
        header = next(csv_reader)

        for line_num, parts in enumerate(csv_reader, 1):
            # 你的csv有21列，确保对应正确
            if len(parts) != 21:
                print(f"Skipped invalid line {line_num}: Incorrect column count ({len(parts)})")
                continue
            try:
                # 原始加速度、陀螺仪、温度
                accel_raw = list(map(float, parts[0:3]))
                gyro_raw = list(map(float, parts[3:6]))
                temp = float(parts[6])
                # 滤波后的加速度、陀螺仪、温度
                accel_filt_raw = list(map(float, parts[7:10]))
                gyro_filt_raw = list(map(float, parts[10:13]))
                temp_filt_raw = float(parts[13])

                # 四元数 W,X,Y,Z
                quat_vals = list(map(float, parts[14:18]))
                # 欧拉角 Roll, Pitch, Yaw
                euler_vals = list(map(float, parts[18:21]))

                # 应用校准
                accel_cal = [(accel_raw[i] - calib['accel_bias'][i]) * calib['accel_scale'][i] for i in range(3)]
                gyro_cal = [gyro_raw[i] - calib['gyro_bias'][i] for i in range(3)]

                # 时间戳，假设采样率200Hz，0.005秒间隔
                t = valid_line_count * 0.005
                time_stamps.append(t)
                valid_line_count += 1

                for i in range(3):
                    accel[i].append(accel_cal[i])
                    gyro[i].append(gyro_cal[i])
                    accel_filt[i].append(accel_filt_raw[i])
                    gyro_filt[i].append(gyro_filt_raw[i])
                    euler[i].append(euler_vals[i])

                temperature.append(temp)
                temp_filt.append(temp_filt_raw)

                for i in range(4):
                    quat[i].append(quat_vals[i])

            except ValueError as e:
                print(f"Skipped invalid line {line_num}: {str(e)}")
                continue

    def calc_stats(data, unit):
        if not all(len(ch) > 0 for ch in data):
            return {'bias': [0.0, 0.0, 0.0], 'std': [0.0, 0.0, 0.0], 'unit': unit}
        means = [sum(ch)/len(ch) for ch in data]
        stds = [(sum((x - mean)**2 for x in ch)/len(ch))**0.5 for ch, mean in zip(data, means)]
        return {'bias': means, 'std': stds, 'unit': unit}

    return {
        'time': time_stamps,
        'accel': calc_stats(accel, 'g'),
        'gyro': calc_stats(gyro, 'dps'),
        'temp_raw': (time_stamps, temperature),
        'accel_raw': (time_stamps, accel),
        'gyro_raw': (time_stamps, gyro),
        'accel_filt': (time_stamps, accel_filt),
        'gyro_filt': (time_stamps, gyro_filt),
        'temp_filt': (time_stamps, temp_filt),
        'quat': (time_stamps, quat),
        'euler': (time_stamps, euler),
    }

def plot_sensor_data_comparison(raw_data, filtered_data, filename, sensor_name):
    plt.figure(figsize=(12, 8))

    time_stamps, raw_sensor_data = raw_data
    _, filtered_sensor_data = filtered_data
    axes = ['X', 'Y', 'Z']

    raw_color = '#ff0000'
    filtered_color = '#00ff00'

    for i in range(3):
        plt.subplot(3,1,i+1)
        plt.plot(time_stamps, raw_sensor_data[i], color=raw_color, linewidth=0.5, alpha=1.0, label='Raw Data')
        plt.plot(time_stamps, filtered_sensor_data[i], color=filtered_color, linewidth=1.5, alpha=0.5, label='Filtered Data')
        plt.ylabel(f'{axes[i]} Axis')
        plt.grid(True, alpha=0.3)
        plt.legend(loc='upper right', fontsize=8)
        if i == 0:
            plt.title(f'{sensor_name} Data Comparison\n(Sampling Rate: 200Hz)')
    plt.xlabel('Time (seconds)')
    plt.tight_layout()
    plt.show()  # 新增：弹出绘图窗口
    plt.savefig(filename, bbox_inches='tight')
    plt.close()

def plot_quat_and_euler(time_stamps, quat_data, euler_data, base_name):
    plt.figure(figsize=(12, 8))
    labels_quat = ['Quat_W', 'Quat_X', 'Quat_Y', 'Quat_Z']
    for i in range(4):
        plt.subplot(4,1,i+1)
        plt.plot(time_stamps, quat_data[i], label=labels_quat[i])
        plt.ylabel(labels_quat[i])
        plt.grid(True, alpha=0.3)
        plt.legend(loc='upper right', fontsize=8)
        if i == 0:
            plt.title('Quaternion Components Over Time')

    plt.xlabel('Time (seconds)')
    plt.tight_layout()
    plt.show()  # 新增：弹出绘图窗口
    plt.savefig(f"{base_name}_quaternion.png")
    plt.close()

    plt.figure(figsize=(12, 6))
    labels_euler = ['Roll', 'Pitch', 'Yaw']
    for i in range(3):
        plt.subplot(3,1,i+1)
        plt.plot(time_stamps, euler_data[i], label=labels_euler[i])
        plt.ylabel(labels_euler[i])
        plt.grid(True, alpha=0.3)
        plt.legend(loc='upper right', fontsize=8)
        if i == 0:
            plt.title('Euler Angles Over Time')

    plt.xlabel('Time (seconds)')
    plt.tight_layout()
    plt.show()  # 新增：弹出绘图窗口
    plt.savefig(f"{base_name}_euler_angles.png")
    plt.close()

def main():
    if len(sys.argv) != 2:
        print("Usage: python process_imu.py <data_file>")
        return

    input_file = sys.argv[1]
    base_name = input_file.rsplit('.', 1)[0]
    results = process_imu_data(input_file)

    plot_sensor_data_comparison(results['accel_raw'], results['accel_filt'], f"{base_name}_accel_comparison.png", "Accelerometer")
    plot_sensor_data_comparison(results['gyro_raw'], results['gyro_filt'], f"{base_name}_gyro_comparison.png", "Gyroscope")
    plot_quat_and_euler(results['time'], results['quat'][1], results['euler'][1], base_name)

    # 保存统计数据
    def save_stats(filename, stats):
        headers = ["Axis", f"Bias ({stats['unit']})", f"Std ({stats['unit']})"]
        rows = [[axis, f"{bias:.4f}", f"{std:.4f}"] for axis, bias, std in zip("XYZ", stats['bias'], stats['std'])]
        with open(filename, 'w') as f:
            csv.writer(f).writerows([headers] + rows)

    save_stats(f"{base_name}_accel_stats.csv", results['accel'])
    save_stats(f"{base_name}_gyro_stats.csv", results['gyro'])

    print("\n=== Sensor Statistics ===")
    def print_stats(name, stats):
        print(f"\n{name}:")
        print(tabulate(
            [[f"{a}-Axis", f"{b:.4f} ± {s:.4f} {stats['unit']}"] 
             for a, b, s in zip("XYZ", stats['bias'], stats['std'])],
            headers=["Axis", "Bias (Mean ± Std)"],
            tablefmt="fancy_grid"
        ))

    print_stats("Accelerometer", results['accel'])
    print_stats("Gyroscope", results['gyro'])

    print("\nGenerated Files:")
    print(f" - {base_name}_accel_comparison.png")
    print(f" - {base_name}_gyro_comparison.png")
    print(f" - {base_name}_quaternion.png")
    print(f" - {base_name}_euler_angles.png")
    print(f" - {base_name}_accel_stats.csv")
    print(f" - {base_name}_gyro_stats.csv")

if __name__ == "__main__":
    main()

