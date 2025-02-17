import serial
from datetime import datetime, timedelta
import csv
import sys

def capture_serial_data(port, baudrate=115200, duration=60):
    """
    从串口捕获指定时长的IMU数据
    :param port: 串口设备路径 (如 '/dev/ttyUSB0')
    :param baudrate: 波特率 (默认115200)
    :param duration: 采集时长（秒）
    :return: (有效数据列表, 无效行数)
    """
    raw_data = []
    invalid_count = 0
    buffer = bytearray()
    
    try:
        # 初始化串口
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1  # 读取超时1秒
        )
        print(f"已连接 {ser.name}，开始采集数据...")

        # 计算结束时间
        end_time = datetime.now() + timedelta(seconds=duration)
        
        while datetime.now() < end_time:
            # 读取串口数据
            data = ser.read_all()
            if data:
                buffer.extend(data)
                
                # 按行分割处理
                while b'\n' in buffer:
                    line_end = buffer.index(b'\n')
                    try:
                        # 先尝试UTF-8解码
                        line = buffer[:line_end].decode("utf-8").strip()
                    except UnicodeDecodeError:
                        # 如果UTF-8解码失败，尝试ISO-8859-1
                        line = buffer[:line_end].decode("ISO-8859-1").strip()
                    
                    del buffer[:line_end+1]  # 删除已处理数据
                    
                    # 验证数据格式
                    if line.count(',') == 6:
                        try:
                            # 转换为整数元组
                            values = tuple(map(int, line.split(',')))
                            raw_data.append(values)
                        except ValueError:
                            invalid_count += 1
                    else:
                        invalid_count += 1

        print("\n数据采集完成")
        return raw_data, invalid_count

    except serial.SerialException as e:
        print(f"串口错误: {str(e)}")
        sys.exit(1)
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()

def save_to_csv(data, filename):
    """保存原始数据到CSV文件"""
    headers = [
        "Accel_X", "Accel_Y", "Accel_Z",
        "Gyro_X", "Gyro_Y", "Gyro_Z",
        "Temperature"
    ]
    
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(headers)
        writer.writerows(data)
    print(f"数据已保存到 {filename}")

if __name__ == "__main__":
    # 配置参数
    SERIAL_PORT = "/dev/ttyUSB0"  # 根据实际设备修改
    BAUD_RATE = 115200            # 根据设备规格修改
    DURATION = 60                 # 采集时长（秒）
    OUTPUT_FILE = "imu_capture.csv"

    # 执行数据采集
    imu_data, invalid_lines = capture_serial_data(SERIAL_PORT, BAUD_RATE, DURATION)
    
    # 保存结果
    save_to_csv(imu_data, OUTPUT_FILE)
    
    # 显示统计信息
    print("\n[采集统计]")
    print(f"有效数据行数: {len(imu_data)}")
    print(f"无效数据行数: {invalid_lines}")
    print(f"采样率: {len(imu_data)/DURATION:.1f} Hz")
    
    # 后续处理建议
    print("\n使用以下命令进行数据分析：")
    print(f"python process_imu.py {OUTPUT_FILE}")

