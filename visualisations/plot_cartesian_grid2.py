import os
import glob
import numpy as np
import matplotlib.pyplot as plt

TARGET_GRID_SIZE = (3000, 3000)

def list_output_files(folder="outputs"):
    # Matches ANY prefix before cartesian_grid_output_*.txt
    pattern = os.path.join(folder, "*cartesian_grid_output_*.txt")
    return sorted(glob.glob(pattern))

def load_grid_from_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    grid = []
    for line in lines:
        row = list(map(float, line.strip().split()))
        grid.append(row)

    return np.array(grid)

def pad_to_target_size(grid, target_size=(2500, 2500)):
    padded_grid = np.full(target_size, np.nan)
    original_rows, original_cols = grid.shape
    target_rows, target_cols = target_size

    # Calculate starting indices to center the data
    start_row = (target_rows - original_rows) // 2
    start_col = (target_cols - original_cols) // 2

    # Determine how many rows/cols to copy (in case data is larger than target)
    rows_to_copy = min(original_rows, target_rows)
    cols_to_copy = min(original_cols, target_cols)

    padded_grid[start_row:start_row + rows_to_copy,
                start_col:start_col + cols_to_copy] = grid[:rows_to_copy, :cols_to_copy]

    return padded_grid

def plot_and_save_grid(grid, title, output_folder="plots"):
    os.makedirs(output_folder, exist_ok=True)
    padded_grid = pad_to_target_size(grid)

    # Get original and padded sizes
    original_rows, original_cols = grid.shape
    padded_rows, padded_cols = padded_grid.shape

    # Calculate starting offset (position of real data in the padded array)
    start_row = (padded_rows - original_rows) // 2
    start_col = (padded_cols - original_cols) // 2

    plt.figure(figsize=(10, 10))
    extent = [0 - start_col, padded_cols - start_col,
              0 - start_row, padded_rows - start_row]
    plt.imshow(padded_grid, cmap='viridis', origin='lower', extent=extent)
    plt.colorbar(label='Value')
    plt.title(title)
    plt.xlabel('X index')
    plt.ylabel('Y index')
    plt.tight_layout()

    output_path = os.path.join(output_folder, f"{title}.png")
    plt.savefig(output_path, dpi=300)
    plt.close()

if __name__ == "__main__":
    files = list_output_files()

    if not files:
        print("No output files found in the 'outputs' folder.")
        exit(1)

    for file in files:
        grid = load_grid_from_file(file)
        filename = os.path.splitext(os.path.basename(file))[0]
        plot_and_save_grid(grid, title=filename)

    print(f"Saved plots for {len(files)} files in the 'plots' folder.")

