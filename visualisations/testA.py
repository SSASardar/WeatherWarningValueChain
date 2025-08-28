import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, FFMpegWriter
import matplotlib.colors as mcolors
#from matplotlib.gridspec as gs
# Create a single colorbar below both grids
from mpl_toolkits.axes_grid1 import make_axes_locatable
from scipy.interpolate import interp1d
import glob
import os



# -----------------------
# Helper functions
# -----------------------
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

def load_VPR(filename):
    """Load VPR points from file: columns = Name, Reflectivity, Height."""
    refl, height = [], []
    with open(filename, "r") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            parts = line.split()
            if len(parts) >= 3:
                r, h = float(parts[1]), float(parts[2])
                refl.append(r)
                height.append(h)
    return np.array(refl), np.array(height)

def load_point_profile(filename):
    """Load point heights: columns = PPI_index, Reflectivity, Height."""
    refl, height = [], []
    with open(filename, "r") as f:
        for line in f:
            if line.startswith("#") or not line.strip():
                continue
            parts = line.split()
            if len(parts) >= 3:   # only 3 columns
                refl.append(float(parts[1]))
                height.append(float(parts[2]))
    return np.array(refl), np.array(height)

# -----------------------
# Load grid files
# -----------------------
disp_files = sorted(glob.glob("outputs/disp_g_*.txt"))
true_files = sorted(glob.glob("outputs/true_g_*.txt"))
point_files = sorted(glob.glob("outputs/heights_point_*.txt"))
vpr_files   = sorted(glob.glob("outputs/VPR_conv_*.txt"))
# for every other file do '[::2]' behind the sorted() function. 
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

# --- Global min/max for consistent color scale ---
vmin = min(np.nanmin(g) for g in disp_grids + true_grids)
vmax = max(np.nanmax(g) for g in disp_grids + true_grids)

# -----------------------
# Load stats
# -----------------------
stats = np.loadtxt("outputs/stats.txt", skiprows=1)
scan_id = stats[:, 0]
MSE = stats[:, 1]
MAE = stats[:, 2]
Bias = stats[:, 3]
total_measured_mm2 = stats[:, 6]
total_true_mm2 = stats[:, 7]
time = scan_id * 5.0  # minutes


# -----------------------
# Setup figure
# -----------------------
fig = plt.figure(figsize=(16, 10))
gs = fig.add_gridspec(2, 3, height_ratios=[2, 1])  # 2 rows, 3 columns

# --- Axes ---
ax_vpr = fig.add_subplot(gs[0, 0])    # left
ax_disp = fig.add_subplot(gs[0, 1])   # middle
ax_true = fig.add_subplot(gs[0, 2])   # right
ax_stats = fig.add_subplot(gs[1, :])  # bottom row for stats timeline

# --- Initial plots ---
im_disp = ax_disp.imshow(disp_grids[0], cmap="Blues", origin="lower", vmin=vmin, vmax=vmax)
im_true = ax_true.imshow(true_grids[0], cmap="Blues", origin="lower", vmin=vmin, vmax=vmax)

# Add red crosses at fixed point
cross_disp, = ax_disp.plot(450, 300, 'rx', markersize=8, markeredgewidth=2)
cross_true, = ax_true.plot(450, 300, 'rx', markersize=8, markeredgewidth=2)

# Titles
ax_disp.set_title("What the radar sees")
ax_true.set_title("Refl. at lowest alt.")


# -----------------------
# Single colorbar below both grids (auto-aligned)
# -----------------------
# Get positions of the disp and true axes
disp_pos = ax_disp.get_position()
true_pos = ax_true.get_position()

# Compute combined horizontal extent
cbar_left = disp_pos.x0
cbar_width = true_pos.x1 - disp_pos.x0
cbar_bottom = disp_pos.y0 - 0.04  # slightly below the axes
cbar_height = 0.02

# Create axis for colorbar
cbar_ax = fig.add_axes([cbar_left, cbar_bottom, cbar_width, cbar_height])
cbar = fig.colorbar(im_disp, cax=cbar_ax, orientation='horizontal')
cbar.set_label("Reflectivity (dBZ)")
cbar.set_ticks(np.linspace(vmin, vmax, 6))
cbar.ax.tick_params(labelsize=10)



# -----------------------
# Stats plots
# -----------------------
ax_stats.plot(time, total_measured_mm2, marker='o', color='purple', label='Measured mm²')
ax_stats.plot(time, total_true_mm2, marker='o', color='orange', label='True mm²')
ax_stats.set_title("True vs Measured over time")
ax_stats.set_xlabel("Time (min)")
ax_stats.set_ylabel("Rainfall rate (mm^2 per sec per m^2)")
ax_stats.legend()
ax_stats.grid(True)

# Vertical line for time
time_line = ax_stats.axvline(time[0], color="black", linestyle="--")

# --- VPR plots ---
vpr_line, = ax_vpr.plot([], [], '-', color="red", label="VPR conv")
vpr_scatter = ax_vpr.scatter([], [], color='red')
profile_line, = ax_vpr.plot([], [], '-', color="blue", label="heights radar scans at")
profile_lines = []

ax_vpr.set_xlim(-10, 60)   # dBZ
ax_vpr.set_ylim(0, 25000)  # meters
ax_vpr.set_xlabel("Reflectivity")
ax_vpr.set_ylabel("Height")
ax_vpr.legend()
ax_vpr.set_title("What radar measures at (x,y)=(450,300)")


# -----------------------
# Pre-plot static stratiform VPR for scan 0000
# -----------------------
refl_s, h_s = load_VPR("outputs/VPR_strat_0000.txt")
if len(refl_s) > 1:
    f_s = interp1d(h_s, refl_s, kind="linear", fill_value="extrapolate")
    h_new_s = np.linspace(min(h_s), max(h_s), 100)
    refl_new_s = f_s(h_new_s)
    ax_vpr.plot(refl_new_s, h_new_s, 'g-', label='VPR strat')  # green line
    #ax_vpr.scatter(refl_s, h_s, color='green', label='Strat points')
    ax_vpr.legend()


# Get current positions
disp_pos = ax_disp.get_position()
true_pos = ax_true.get_position()
vpr_pos = ax_vpr.get_position()  # reference top height

# Move disp and true axes higher to align with VPR top
new_top = vpr_pos.y1
height = disp_pos.height  # keep original height
ax_disp.set_position([disp_pos.x0, new_top - height, disp_pos.width, height])
ax_true.set_position([true_pos.x0, new_top - height, true_pos.width, height])
# Compute new colorbar position
cbar_bottom = ax_disp.get_position().y0 - 0.09  # a bit more space than before
cbar_height = 0.02
cbar_ax.set_position([disp_pos.x0, cbar_bottom, true_pos.x1 - disp_pos.x0, cbar_height])


def update(frame):
    # --- grids ---
    im_disp.set_data(disp_grids[frame].T)
    im_true.set_data(true_grids[frame].T)
    timestamp_min = (frame) * 5
    ax_disp.set_title(f"What the radar sees - {timestamp_min} min")
    ax_true.set_title(f"Refl. at lowest alt. - {timestamp_min} min")

    # --- remove old point profile lines ---
    for obj in profile_lines:
        obj.remove()
    profile_lines.clear()

    # --- point heights as horizontal dashed lines only ---
    _, h_p = load_point_profile(point_files[frame])
    for h in h_p:
        # horizontal dashed line using ax.plot (Line2D)
        line, = ax_vpr.plot([-10, 60], [h, h], 'b--', alpha=0.6)
        profile_lines.append(line)

    # --- VPR conv points + interpolation ---
    refl_v, h_v = load_VPR(vpr_files[frame])
    if len(refl_v) > 1:
        f = interp1d(h_v, refl_v, kind="linear", fill_value="extrapolate")
        h_new = np.linspace(min(h_v), max(h_v), 100)
        refl_new = f(h_new)
        vpr_line.set_data(refl_new, h_new)
        vpr_scatter.set_offsets(np.c_[refl_v, h_v])

    # --- stats timeline ---
    time_line.set_xdata([timestamp_min])

    # Return all updated artists
    return [im_disp, im_true, vpr_line, vpr_scatter, time_line] + profile_lines + [cross_disp, cross_true]







# -----------------------
# Animate
# -----------------------
ani = FuncAnimation(fig, update, frames=num_frames, blit=False)

os.makedirs("outputs", exist_ok=True)
writer = FFMpegWriter(fps=5, metadata=dict(artist='Radar Anim'), bitrate=1800)
ani.save("outputs/dashboard_animation.mp4", writer=writer)
print("Saved animation to outputs/dashboard_animation.mp4")

