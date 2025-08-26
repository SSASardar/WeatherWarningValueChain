import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, FFMpegWriter
import glob
import os

def load_grid(filename):
    """Load a 2D grid from file, skipping comment lines starting with #."""
    values = []
    with open(filename, "r") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            row = [float(tok) if tok.lower() != "nan" else np.nan for tok in line.split()]
            values.append(row)
    return np.array(values)

def pad_grid(grid, target_shape):
    """Pad a 2D grid with NaN to match target_shape (rows, cols)."""
    padded = np.full(target_shape, np.nan)
    rows, cols = grid.shape
    padded[:rows, :cols] = grid
    return padded

def downsample(grid, factor=2):
    """Downsample a 2D grid by taking every 'factor' row/col."""
    return grid[::factor, ::factor]

# --- Load and select every 2nd file ---
disp_files = sorted(glob.glob("outputs/disp_g_*.txt"))[::2]
true_files = sorted(glob.glob("outputs/true_g_*.txt"))[::2]

disp_grids_raw = [load_grid(f) for f in disp_files]
true_grids_raw = [load_grid(f) for f in true_files]

# --- Find max shape and pad ---
max_rows = max(max(g.shape[0] for g in disp_grids_raw),
               max(g.shape[0] for g in true_grids_raw))
max_cols = max(max(g.shape[1] for g in disp_grids_raw),
               max(g.shape[1] for g in true_grids_raw))

disp_grids = [downsample(pad_grid(g, (max_rows, max_cols)), factor=2) for g in disp_grids_raw]
true_grids = [downsample(pad_grid(g, (max_rows, max_cols)), factor=2) for g in true_grids_raw]

num_frames = len(disp_grids)

# --- Determine global min/max for consistent color scale ---
vmin = min(np.nanmin(g) for g in disp_grids + true_grids)
vmax = max(np.nanmax(g) for g in disp_grids + true_grids)

print(f"Animation will have {num_frames} frames, grid shape: {disp_grids[0].shape}")

# --- Setup figure and axes ---
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
im1 = ax1.imshow(disp_grids[0], cmap="turbo", origin="lower", vmin=vmin, vmax=vmax)
im2 = ax2.imshow(true_grids[0], cmap="turbo", origin="lower", vmin=vmin, vmax=vmax)
ax1.set_title("Display Grid")
ax2.set_title("True Grid")
fig.colorbar(im1, ax=ax1, fraction=0.046, pad=0.04, label="dBZ")
fig.colorbar(im2, ax=ax2, fraction=0.046, pad=0.04, label="dBZ")

def update(frame):
    timestamp_min = (frame * 2 + 1) * 5  # Every 2nd file, +1, times 5 minutes
    im1.set_data(disp_grids[frame].T)
    im2.set_data(true_grids[frame].T)
    ax1.set_title(f"Display Grid - {timestamp_min} min")
    ax2.set_title(f"True Grid - {timestamp_min} min")
    return [im1, im2]

# --- Create animation ---
ani = FuncAnimation(fig, update, frames=num_frames, blit=True)

# --- Save as MP4 ---
os.makedirs("outputs", exist_ok=True)
writer = FFMpegWriter(fps=5, metadata=dict(artist='Radar Anim'), bitrate=1800)
ani.save("outputs/side_by_side_animation.mp4", writer=writer)
print("Saved animation to outputs/side_by_side_animation.mp4")

