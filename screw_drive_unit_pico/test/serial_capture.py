import serial
from datetime import datetime, timedelta
import csv

def capture_serial_data(port, baudrate=115200, duration=30):
    raw_data = []
    filtered_data = []
    quaternion_data = []
    euler_data = []
    invalid_count = 0
    buffer = bytearray()
    ser = None
    line_counter = 0
    temp_group = []
    sync_completed = False  # 分组同步标记

    try:
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.1  # 缩短超时，适应高频数据
        )
        print(f"Connected to {ser.name}, starting data acquisition...")

        end_time = datetime.now() + timedelta(seconds=duration)

        # 第一步：同步到第一组数据的起始（等待第一个7列原始数据行）
        print("Syncing to data groups...")
        while not sync_completed and datetime.now() < end_time:
            data = ser.read(1)  # 逐字节读取，避免数据堆积
            if data:
                buffer.extend(data)
                # 统一换行符：替换\r\n为\n，移除单独的\r
                buffer = buffer.replace(b'\r\n', b'\n').replace(b'\r', b'')
                if b'\n' in buffer:
                    line_end = buffer.index(b'\n')
                    line = buffer[:line_end].decode("utf-8", errors="replace").strip()
                    del buffer[:line_end + 1]
                    parts = line.split(',')
                    # 找到第一个7列且能转换为数值的行，作为分组起始（转换为float后存入）
                    if len(parts) == 7:
                        try:
                            float_values = [float(p) for p in parts]  # 强制转换为float
                            sync_completed = True
                            temp_group = [float_values]  # 存入float列表
                            print("Sync completed, starting data capture...")
                        except:
                            pass

        # 第二步：采集并解析数据（已同步分组）
        while datetime.now() < end_time and sync_completed:
            data = ser.read(1)
            if data:
                buffer.extend(data)
                buffer = buffer.replace(b'\r\n', b'\n').replace(b'\r', b'')  # 处理换行符

                while b'\n' in buffer:
                    line_end = buffer.index(b'\n')
                    line = buffer[:line_end].decode("utf-8", errors="replace").strip()
                    del buffer[:line_end + 1]
                    line_counter += 1

                    if not line:
                        invalid_count += 1
                        continue

                    parts = line.split(',')

                    # 按设备固定格式校验列数（7→7→4→3）
                    expected_lengths = [7, 7, 4, 3]
                    current_pos = len(temp_group)  # 当前分组中的位置（0-3）
                    if len(parts) != expected_lengths[current_pos]:
                        invalid_count += 1
                        # 重置分组，重新同步
                        temp_group = []
                        sync_completed = False
                        print(f"Group sync lost, re-syncing... (invalid line: {line})")
                        # 重新等待第一行7列数据
                        while not sync_completed and datetime.now() < end_time:
                            data = ser.read(1)
                            if data:
                                buffer.extend(data)
                                buffer = buffer.replace(b'\r\n', b'\n').replace(b'\r', b'')
                                if b'\n' in buffer:
                                    le = buffer.index(b'\n')
                                    ln = buffer[:le].decode("utf-8", errors="replace").strip()
                                    del buffer[:le + 1]
                                    p = ln.split(',')
                                    if len(p) == 7:
                                        try:
                                            float_p = [float(pp) for pp in p]  # 转换为float
                                            sync_completed = True
                                            temp_group = [float_p]
                                            print("Resynced successfully")
                                        except:
                                            pass
                        continue

                    # 转换为数值并加入分组（确保是float类型）
                    try:
                        values = [float(p) for p in parts]  # 强制转换为float
                        temp_group.append(values)
                    except ValueError as e:
                        invalid_count += 1
                        temp_group = []
                        sync_completed = False
                        print(f"Value error, re-syncing: {e}")
                        # 重新同步
                        while not sync_completed and datetime.now() < end_time:
                            data = ser.read(1)
                            if data:
                                buffer.extend(data)
                                buffer = buffer.replace(b'\r\n', b'\n').replace(b'\r', b'')
                                if b'\n' in buffer:
                                    le = buffer.index(b'\n')
                                    ln = buffer[:le].decode("utf-8", errors="replace").strip()
                                    del buffer[:le + 1]
                                    p = ln.split(',')
                                    if len(p) == 7:
                                        try:
                                            float_p = [float(pp) for pp in p]  # 转换为float
                                            sync_completed = True
                                            temp_group = [float_p]
                                            print("Resynced successfully")
                                        except:
                                            pass
                        continue

                    # 分组完整（4行），保存数据（确保都是float列表）
                    if len(temp_group) == 4:
                        raw_data.append(temp_group[0])
                        filtered_data.append(temp_group[1])
                        quaternion_data.append(temp_group[2])
                        euler_data.append(temp_group[3])
                        temp_group = []  # 重置分组

    except serial.SerialException as e:
        print(f"Serial port error: {str(e)}")
    finally:
        if ser and ser.is_open:
            ser.close()

    print("\nData acquisition completed")
    return raw_data, filtered_data, quaternion_data, euler_data, invalid_count

def save_imu_data(raw_data, filtered_data, quaternion_data, euler_data, filename):
    headers = [
        "Accel_X_raw", "Accel_Y_raw", "Accel_Z_raw",
        "Gyro_X_raw", "Gyro_Y_raw", "Gyro_Z_raw", "Temp_raw",
        "Accel_X_filt", "Accel_Y_filt", "Accel_Z_filt",
        "Gyro_X_filt", "Gyro_Y_filt", "Gyro_Z_filt", "Temp_filt",
        "Quat_W", "Quat_X", "Quat_Y", "Quat_Z",
        "Euler_Roll", "Euler_Pitch", "Euler_Yaw"
    ]

    with open(filename, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(headers)

        for raw, filt, quat, euler in zip(raw_data, filtered_data, quaternion_data, euler_data):
            # 补全可能缺失的数据（确保是float）
            raw_padded = raw[:7] + [0.0]*(7 - len(raw)) if len(raw) <7 else raw
            filt_padded = filt[:7] + [0.0]*(7 - len(filt)) if len(filt) <7 else filt
            quat_padded = quat[:4] + [0.0]*(4 - len(quat)) if len(quat) <4 else quat
            euler_padded = euler[:3] + [0.0]*(3 - len(euler)) if len(euler) <3 else euler

            # 格式化输出（此时所有数据均为float）
            row = [
                *[f"{v:.2f}" for v in raw_padded[:6]], f"{raw_padded[6]:.2f}",
                *[f"{v:.2f}" for v in filt_padded[:6]], f"{filt_padded[6]:.2f}",
                *[f"{v:.6f}" for v in quat_padded],
                *[f"{v:.2f}" for v in euler_padded]
            ]
            writer.writerow(row)

if __name__ == "__main__":
    SERIAL_PORT = "/dev/ttyUSB0"
    BAUD_RATE = 115200
    DURATION = 30

    raw_data, filtered_data, quaternion_data, euler_data, invalid_lines = capture_serial_data(SERIAL_PORT, BAUD_RATE, DURATION)
    save_imu_data(raw_data, filtered_data, quaternion_data, euler_data, "sensor_data3.csv")

    print("\n[Acquisition Report]")
    print(f"Raw samples: {len(raw_data)}")
    print(f"Filtered samples: {len(filtered_data)}")
    print(f"Quaternion samples: {len(quaternion_data)}")
    print(f"Euler samples: {len(euler_data)}")
    print(f"Invalid lines: {invalid_lines}")
    if DURATION > 0:
        print(f"Effective rate: {min(len(raw_data), len(filtered_data), len(quaternion_data), len(euler_data)) / DURATION:.1f} Hz")
    print("\nGenerated file:\n- sensor_data3.csv")
