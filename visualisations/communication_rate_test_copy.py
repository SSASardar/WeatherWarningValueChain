import numpy as np
import matplotlib.pyplot as plt

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
            rain_diff_per_time[t] = R_reg - R_avg  # signed difference
        else:
            rain_diff_per_time[t] = 0

    cumulative_error = np.cumsum(np.abs(rain_diff_per_time)) * timestep_min / 60.0
    #cumulative_error = np.cumsum(np.abs(rain_diff_per_time))/n_times
    return time_labels, rain_diff_per_time, cumulative_error

# -------------------------
# Load files
# -------------------------
time_1min, diff_1min, cum_1min = process_reflectivity_files(
    "outputs/reflectivity_profile_1min.txt",
    "outputs/reflectivity_profile_avg_1min.txt",
    timestep_min=1.0
)

time_5min, diff_5min, cum_5min = process_reflectivity_files(
    "outputs/reflectivity_profile_5min.txt",
    "outputs/reflectivity_profile_avg_5min.txt",
    timestep_min=5.0
)

# Colors
color_1min = "#808080"  # orange
color_5min = "#800080"  # blue


# -------------------------
# Plot 2x1: top=rate, bottom=accumuation
# -------------------------
fig, axs = plt.subplots(2, 1, figsize=(12, 10), sharex=True)

# -------------------------
# Top: Instantaneous Rain Rate Difference
# -------------------------
axs[0].plot(time_1min, diff_1min, color=color_1min, lw=2, label="1-min", alpha=0.9, linestyle = "--")
axs[0].plot(time_5min, diff_5min, color=color_5min, lw=2, label="5-min",alpha=0.9)
axs[0].set_title("Instantaneous Rain Rate Difference (less or more than measured)")
axs[0].set_ylabel("Rain Rate Diff (mm/h)")
axs[0].grid(True)
axs[0].legend()

# -------------------------
# Bottom: Cumulative Rainfall Error
# -------------------------
axs[1].plot(time_1min, cum_1min, color=color_1min, lw=2, label="1-min", alpha = 0.9, linestyle = "--")
axs[1].plot(time_5min, cum_5min, color=color_5min, lw=2, label="5-min", alpha = 0.9)
axs[1].set_title("Cumulative total  Rainfall accumulation Error")
axs[1].set_xlabel("Time (minutes)")
axs[1].set_ylabel("Cumulative Error (mm)")
axs[1].grid(True)
axs[1].legend()

plt.tight_layout()
plt.show()
