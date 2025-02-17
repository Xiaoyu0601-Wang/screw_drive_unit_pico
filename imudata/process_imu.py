import sys
import csv
import matplotlib.pyplot as plt
from collections import defaultdict
from tabulate import tabulate

# 设置纯英文字体
plt.rcParams.update({
    'font.sans-serif': ['Arial'],
    'axes.unicode_minus': False
})

def process_imu_data(filename):
    # Initialize data storage
    accel = [[] for _ in range(3)]
    gyro = [[] for _ in range(3)]
    temperature = []
    time_stamps = []
    temp_windows = defaultdict(list)
    
    with open(filename, 'r') as f:
        for line_num, line in enumerate(f):
            try:
                parts = list(map(int, line.strip().split(',')))
                if len(parts) != 7:
                    raise ValueError
                
                # Store sensor data
                for i in range(3):
                    accel[i].append(parts[i])
                    gyro[i].append(parts[i+3])
                
                # Timestamp calculation
                t = line_num * 0.005  # 200Hz = 5ms per frame
                time_stamps.append(t)
                
                # Temperature windowing
                window_idx = int(t * 10)
                temp_windows[window_idx].append(parts[6])
                
                temperature.append(parts[6])
            
            except (ValueError, IndexError):
                print(f"Skipped invalid line: {line_num+1}")
                continue

    # Calculate averages
    def calc_avg(data): return [sum(ch)/len(ch) for ch in data]
    
    # Process temperature windows
    temp_avg = []
    window_centers = []
    for idx in sorted(temp_windows.keys()):
        values = temp_windows[idx]
        if values:
            window_start = idx * 0.1
            window_center = window_start + 0.05
            avg = sum(values)/len(values)
            temp_avg.append(avg)
            window_centers.append(window_center)

    return {
        'accel_avg': calc_avg(accel),
        'gyro_avg': calc_avg(gyro),
        'temp_raw': (time_stamps, temperature),
        'accel_raw': (time_stamps, accel),
        'gyro_raw': (time_stamps, gyro),
        'temp_avg': (window_centers, temp_avg)
    }

def plot_sensor_data(data, filename, sensor_name):
    """Plot sensor data with English labels"""
    plt.figure(figsize=(12, 8))
    
    time_stamps = data[0]
    sensor_data = data[1]
    axes = ['X', 'Y', 'Z']
    colors = ['#1f77b4', '#ff7f0e', '#2ca02c']
    
    for i in range(3):
        plt.subplot(3, 1, i+1)
        plt.plot(time_stamps, sensor_data[i], 
                color=colors[i], linewidth=0.5)
        plt.ylabel(f'{axes[i]} Axis (LSB)')
        plt.grid(True, alpha=0.3)
        if i == 0:
            plt.title(f'{sensor_name} Sensor Raw Data @200Hz')
    
    plt.xlabel('Time (seconds)')
    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()

def plot_temperature(data, filename):
    """Plot temperature data with English labels"""
    plt.figure(figsize=(12, 6))
    
    # Raw data plot
    plt.plot(data['temp_raw'][0], data['temp_raw'][1], 
            alpha=0.3, linewidth=0.5, label='Raw Data')
    
    # Window averages
    plt.scatter(data['temp_avg'][0], data['temp_avg'][1],
               color='red', s=20, zorder=3, label='100ms Average')
    
    plt.title('Temperature Sensor Data\n(Raw @200Hz + 100ms Window Average)')
    plt.xlabel('Time (seconds)')
    plt.ylabel('Temperature (LSB)')
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()

def write_csv(filename, headers, rows):
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        writer.writerows(rows)

def main():
    if len(sys.argv) != 2:
        print("Usage: python process_imu.py <data_file>")
        return

    input_file = sys.argv[1]
    base_name = input_file.rsplit('.', 1)[0]
    results = process_imu_data(input_file)

    # Generate plots
    plot_files = {
        'temperature': f"{base_name}_temperature.png",
        'acceleration': f"{base_name}_acceleration.png",
        'gyroscope': f"{base_name}_gyroscope.png"
    }
    
    plot_temperature(results, plot_files['temperature'])
    plot_sensor_data(results['accel_raw'], plot_files['acceleration'], "Acceleration")
    plot_sensor_data(results['gyro_raw'], plot_files['gyroscope'], "Gyroscope")

    # Save CSV files
    csv_files = {
        'accel': f"{base_name}_accel.csv",
        'gyro': f"{base_name}_gyro.csv",
        'temp': f"{base_name}_temp.csv"
    }
    
    write_csv(csv_files['accel'],
             ["Axis", "Average (LSB)"],
             [[axis, round(v, 2)] for axis, v in zip(["X", "Y", "Z"], results['accel_avg'])])
    
    write_csv(csv_files['gyro'],
             ["Axis", "Average (LSB)"],
             [[axis, round(v, 2)] for axis, v in zip(["X", "Y", "Z"], results['gyro_avg'])])
    
    # Terminal output
    print("\nAccelerometer Averages:")
    print(tabulate(
        [[f"{a}-Axis", f"{v:.2f} LSB"] for a, v in zip("XYZ", results['accel_avg'])],
        headers=["Axis", "Average"],
        tablefmt="fancy_grid"
    ))

    print("\nGenerated Files:")
    for name in plot_files.values():
        print(f"  - {name}")
    for name in csv_files.values():
        print(f"  - {name}")

if __name__ == "__main__":
    main()
