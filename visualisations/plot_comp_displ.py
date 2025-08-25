import numpy as np
import matplotlib.pyplot as plt
import glob
import os
from mpl_toolkits.axes_grid1 import make_axes_locatable

def load_display_grid(filename):
    values = []
    with open(filename, "r") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            row = []
            for token in line.split():
                if token.lower() == "nan":
                    row.append(np.nan)
                else:
                    row.append(float(token))
            values.append(row)
    return np.array(values)

def compute_metrics(grid, true_grid):
    mask = ~np.isnan(grid) & ~np.isnan(true_grid)
    diff = grid[mask] - true_grid[mask]
    mse = np.mean(diff**2)
    mae = np.mean(np.abs(diff))
    bias = np.mean(diff)
    return mse, mae, bias

def plot_grids_2x2(files, true_file, out_ext="png"):
    true_grid = load_display_grid(true_file)
    grids = [true_grid] + [load_display_grid(f) for f in files]
    base_names = ["TRUE"] + [os.path.splitext(os.path.basename(f))[0] for f in files]
    n_grids = len(grids)

    os.makedirs("plots", exist_ok=True)
    outfile = os.path.join("plots", f"{'_'.join(base_names)}.{out_ext}")

    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    axes = axes.flatten()

    # Find min and max for consistent color scale across all grids
    all_values = np.concatenate([g[~np.isnan(g)].flatten() for g in grids])
    vmin, vmax = np.min(all_values), np.max(all_values)

    for i, (ax, grid, name) in enumerate(zip(axes, grids, base_names)):
        im = ax.imshow(grid.T, origin="lower", cmap="turbo", interpolation="nearest", vmin=vmin, vmax=vmax)
        ax.set_title(name)
        ax.set_xlabel("X")
        ax.set_ylabel("Y")

        if i > 0:
            mse, mae, bias = compute_metrics(grid, true_grid)
            ax.text(
                0.5, -0.2,
                f"MSE: {mse:.2f}\nMAE: {mae:.2f}\nBias: {bias:.2f}",
                transform=ax.transAxes,
                ha="center",
                va="top",
                fontsize=10
            )

    # Remove empty subplots if fewer than 4 grids
    for j in range(n_grids, 4):
        fig.delaxes(axes[j])

    # Create a colorbar axis that spans all subplots on the right
    from mpl_toolkits.axes_grid1 import make_axes_locatable
    divider = make_axes_locatable(axes[-1])
    cax = divider.append_axes("right", size="5%", pad=0.1)
    cbar = fig.colorbar(im, cax=cax)
    cbar.set_label("Reflectivity (dBZ)")

    plt.tight_layout()
    plt.savefig(outfile, dpi=150)
    plt.close()
    print(f"Saved combined 2x2 plot with full colorbar: {outfile}")

if __name__ == "__main__":
    files = sorted(glob.glob("outputs/disp_grid*.txt"))
    true_file = "outputs/disp_grid_true.txt"
    files = [f for f in files if "true" not in f][:3]  # 3 predicted grids + TRUE

    if files and os.path.exists(true_file):
        plot_grids_2x2(files, true_file, out_ext="png")
    else:
        print("No input files or true file found.")
