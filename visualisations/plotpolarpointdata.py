import os
import re
import numpy as np
import matplotlib.pyplot as plt

def parse_scan_block(lines):
    meta = {}
    data = []

    key_value_re = re.compile(r'(\w+(?:\.\w+)*)=([\w\.\-]+)')

    reading_grid_data = False
    grid_data_accum = []

    for line in lines:
        line = line.strip()
        if not line or line.startswith("==="):
            continue

        if reading_grid_data:
            if key_value_re.match(line):
                reading_grid_data = False
                data = list(map(float, " ".join(grid_data_accum).split()))
            else:
                grid_data_accum.append(line)
                continue

        if line.startswith("grid.data="):
            reading_grid_data = True
            grid_data_accum.append(line[len("grid.data="):].strip())
            continue

        if not reading_grid_data:
            match = key_value_re.match(line)
            if match:
                key, val = match.groups()
                try:
                    meta[key] = float(val)
                except ValueError:
                    meta[key] = val

    if reading_grid_data:
        data = list(map(float, " ".join(grid_data_accum).split()))

    num_ranges = int(meta.get('box.num_ranges', meta.get('num_ranges', 0)))
    num_angles = int(meta.get('box.num_angles', meta.get('num_angles', 0)))

    if num_ranges * num_angles != len(data):
        raise ValueError(f"Data length mismatch: expected {num_ranges*num_angles}, got {len(data)}")

    data_array = np.array(data).reshape((num_ranges, num_angles))
    return meta, data_array


def plot_polar(data, meta, output_path):
    # Calculate min range in distance units:
    min_range_gate = meta.get('box.min_range_gate', 0)
    range_res = meta.get('box.range_resolution', 1)
    min_range = min_range_gate * range_res  # <-- Important change here!

    # Max range directly from radar.maximum_range
    max_range = meta.get('radar.maximum_range', None)

    min_angle_deg = meta.get('box.min_angle', 0)
    angular_res_deg = meta.get('box.angular_resolution', 1)

    num_ranges, num_angles = data.shape

    # Range bins in distance units starting from min_range
    r_data = min_range + np.arange(num_ranges) * range_res

    # Angles converted to radians
    theta_deg = (min_angle_deg + np.arange(num_angles)) * angular_res_deg
    theta_rad = np.deg2rad(theta_deg)

    # Meshgrid of polar coordinates (radii, angles)
    R, Theta = np.meshgrid(r_data, theta_rad, indexing='ij')

    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'}, figsize=(8, 6))
    pcm = ax.pcolormesh(Theta, R, data, shading='auto', cmap='viridis')
    plt.colorbar(pcm, label="Sample Value")

    # Set radius limits properly
    if max_range:
        ax.set_ylim(0, max_range)
    else:
        ax.set_ylim(0, r_data[-1])

    ax.set_theta_offset(0)
    ax.set_theta_direction(1)
    ax.set_title(f"Radar Scan {int(meta.get('scan.index', 0))} Polar Plot")
    plt.tight_layout()
    plt.savefig(output_path)
    print(f"✅ Plot saved to: {output_path}")
    plt.close()


def main():
    data_folder = os.path.abspath("outputs")
    plot_folder = os.path.abspath("plots")
    files = [f for f in os.listdir(data_folder) if f.endswith(".txt")]

    if not files:
        print("No .txt files found in outputs/")
        return

    print("Available files:")
    for i, file in enumerate(files):
        print(f"{i + 1}: {file}")

    try:
        choice = int(input("Select file number to plot scans from: ")) - 1
        selected_file = files[choice]
    except (IndexError, ValueError):
        print("Invalid selection.")
        return

    file_path = os.path.join(data_folder, selected_file)
    
    with open(file_path, 'r') as f:
        lines = f.readlines()

    # Split file into scan blocks
    scans = []
    current_scan_lines = []
    inside_scan = False

    for line in lines:
        if "=== BEGIN RADAR_SCAN ===" in line:
            inside_scan = True
            current_scan_lines = []
        elif "=== END RADAR_SCAN ===" in line:
            inside_scan = False
            scans.append(current_scan_lines)
        elif inside_scan:
            current_scan_lines.append(line)

    if not scans:
        print("No radar scans found in the file.")
        return

    for i, scan_lines in enumerate(scans):
        try:
            meta, data = parse_scan_block(scan_lines)
        except Exception as e:
            print(f"Error parsing scan {i}: {e}")
            continue

        plot_filename = f"{os.path.splitext(selected_file)[0]}_scan_{int(meta.get('scan.index', i))}.png"
        plot_path = os.path.join(plot_folder, plot_filename)
        plot_polar(data, meta, plot_path)


if __name__ == "__main__":
    main()

