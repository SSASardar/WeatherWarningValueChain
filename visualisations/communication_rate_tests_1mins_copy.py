import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1.inset_locator import inset_axes

# -------------------------
# Marshall–Palmer Z–R relation
# -------------------------
def dbz_to_rainrate(dbz):
    Z = 10 ** (dbz / 10)
    R = (Z / 200.0) ** (1 / 1.6)
    return R

# -------------------------
# Process reflectivity files
# -------------------------
def process_reflectivity_files(file_regular, file_avg, timestep_min):
    data_regular = np.loadtxt(file_regular, skiprows=1)
    data_avg = np.loadtxt(file_avg, skiprows=1)

    reflect_regular = data_regular[:, 1:]
    reflect_avg = data_avg[:, 1:]
    n_heights, n_times = reflect_regular.shape

    with open(file_regular, "r") as f:
        header = f.readline().strip().split("\t")[1:]
    time_labels = [int(h[1:]) for h in header]

    rain_diff_per_time = np.zeros(n_times)
    for t in range(n_times):
        mask_reg = reflect_regular[:, t] > 0
        mask_avg = reflect_avg[:, t] > 0
        if np.any(mask_reg) and np.any(mask_avg):
            lowest_idx_reg = np.argmax(mask_reg)
            lowest_idx_avg = np.argmax(mask_avg)
            lowest_idx = min(lowest_idx_reg, lowest_idx_avg)
            R_reg = dbz_to_rainrate(reflect_regular[lowest_idx, t])
            R_avg = dbz_to_rainrate(reflect_avg[lowest_idx, t])
            rain_diff_per_time[t] = R_reg - R_avg
        else:
            rain_diff_per_time[t] = 0

    cumulative_error = np.cumsum(np.abs(rain_diff_per_time)) * timestep_min / 60.0
    return time_labels, rain_diff_per_time, cumulative_error

# -------------------------
# File configurations for 1-min only
# -------------------------
file_configs_1min = [
    ("outputs/reflectivity_profile_1min.txt", "outputs/reflectivity_profile_avg_1min.txt", "1-min 5 samples", "dashed", "#808080"),
    ("outputs/reflectivity_profile_1min.txt", "outputs/Breflectivity_profile_avg_1min.txt", "1-min 2 samples", "dotted", "#808080"),
    ("outputs/reflectivity_profile_1min.txt", "outputs/Creflectivity_profile_avg_1min.txt", "1-min 1 sample", "solid", "#808080"),
]

# -------------------------
# Process all 1-min files
# -------------------------
results = []
for file_regular, file_avg, label, linestyle, color in file_configs_1min:
    time_labels, diff, cum = process_reflectivity_files(file_regular, file_avg, timestep_min=1.0)
    results.append((time_labels, diff, cum, label, linestyle, color))

# -------------------------
# Plot setup
# -------------------------
fig, axs = plt.subplots(2, 1, figsize=(12, 10), sharex=True)

# -------------------------
# Top: Instantaneous Rain Rate Difference
# -------------------------
for time_labels, diff, cum, label, linestyle, color in results:
    axs[0].plot(time_labels, diff, label=label, linestyle=linestyle, color=color, lw=2, alpha=0.9)

axs[0].set_title("Instantaneous Rain Rate Difference (- = overestimation, + = underestimation)")
axs[0].set_ylabel("Rain Rate Diff (mm/h)")
axs[0].grid(True)
axs[0].legend()

# -------------------------
# Inset: zoom on small differences for B and C
# -------------------------
ax_inset = inset_axes(axs[0], width="40%", height="40%", loc='upper left', borderpad=2)

# Plot only B and C differences in the inset
for time_labels, diff, cum, label, linestyle, color in results:
    if "2" in label or "1 sample" in label:  # match B & C labels
        ax_inset.plot(time_labels, diff, label=label, linestyle=linestyle, color=color, lw=2, alpha=0.9)

# Set x limits to match main plot
ax_inset.set_xlim(axs[0].get_xlim())

# Symmetric y-limits around zero
bc_min = min(np.min(diff) for _, diff, _, label, *_ in results if "2" in label or "1 sample" in label)
bc_max = max(np.max(diff) for _, diff, _, label, *_ in results if "2" in label or "1 sample" in label)
max_abs = max(abs(bc_min), abs(bc_max))
ax_inset.set_ylim(-max_abs, 0)

# Move y-axis to the right
ax_inset.yaxis.set_label_position("right")
ax_inset.yaxis.tick_right()

ax_inset.set_title("Zoom: 2 and 1 sample differences", fontsize=10)
ax_inset.grid(True)
ax_inset.tick_params(axis='both', labelsize=8)

# -------------------------
# Bottom: Cumulative Rainfall Error
# -------------------------
for time_labels, diff, cum, label, linestyle, color in results:
    axs[1].plot(time_labels, cum, label=label, linestyle=linestyle, color=color, lw=2, alpha=0.9)

axs[1].set_title("Cumulative Rainfall Error")
axs[1].set_xlabel("Time (minutes)")
axs[1].set_ylabel("Cumulative Error (mm)")
axs[1].grid(True)
axs[1].legend()

plt.tight_layout()
plt.show()
