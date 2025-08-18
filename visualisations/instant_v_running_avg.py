import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# -------------------------
# Marshall–Palmer Z–R relation helpers
# -------------------------
def dbz_to_rainrate(dbz):
    """Convert dBZ to rainfall rate R (mm/h) using MP relation Z=200 R^1.6."""
    Z = 10 ** (dbz / 10)  # from dBZ to linear Z
    R = (Z / 200.0) ** (1 / 1.6)
    return R

# -------------------------
# Load regular and avg profiles
# -------------------------
file_regular = "outputs/reflectivity_profile.txt"
file_avg = "outputs/reflectivity_profile_avg.txt"

data_regular = np.loadtxt(file_regular, skiprows=1)
data_avg = np.loadtxt(file_avg, skiprows=1)

heights = data_regular[:, 0]
reflect_regular = data_regular[:, 1:]
reflect_avg = data_avg[:, 1:]
n_heights, n_times = reflect_regular.shape

# Extract time labels from header
with open(file_regular, "r") as f:
    header = f.readline().strip().split("\t")[1:]
time_labels = [int(h[1:]) for h in header]  # minutes

# -------------------------
# Compute lowest non-zero reflectivity per time, rainfall difference
# -------------------------
rain_diff_per_time = np.zeros(n_times)
for t in range(n_times):
    mask_reg = reflect_regular[:, t] > 0
    mask_avg = reflect_avg[:, t] > 0
    if np.any(mask_reg) and np.any(mask_avg):
        lowest_idx_reg = np.argmax(mask_reg)
        lowest_idx_avg = np.argmax(mask_avg)
        lowest_idx = min(lowest_idx_reg, lowest_idx_avg)
        dbz_reg = reflect_regular[lowest_idx, t]
        dbz_avg = reflect_avg[lowest_idx, t]
        R_reg = dbz_to_rainrate(dbz_reg)
        R_avg = dbz_to_rainrate(dbz_avg)
        rain_diff_per_time[t] = (R_reg - R_avg)
    else:
        rain_diff_per_time[t] = 0

cumulative_rain_diff = np.cumsum(abs(rain_diff_per_time))
cumulative_rain_diffA = np.cumsum((rain_diff_per_time))
rainfall_acc_error = cumulative_rain_diff * 5.0/60.0;
# -------------------------
# Set up figure with 3 plots
# -------------------------
fig, (ax_profiles,  ax_inst_diff, ax_cum_diff) = plt.subplots(
    3, 1, figsize=(10, 10), gridspec_kw={'height_ratios': [3, 1, 1]}
)

# Profiles animation plot
line_reg, = ax_profiles.plot(reflect_regular[:, 0], heights, label="true VPR", color="blue", lw=2)
line_avg, = ax_profiles.plot(reflect_avg[:, 0], heights, label="averaged/measured VPR", color="red", lw=2)
ax_profiles.set_xlim(min(np.min(reflect_regular), np.min(reflect_avg)),
                     max(np.max(reflect_regular), np.max(reflect_avg)))
ax_profiles.set_ylim(np.min(heights), np.max(heights))
ax_profiles.set_xlabel("Reflectivity (dBZ)")
ax_profiles.set_ylabel("Height (m)")
ax_profiles.legend()
ax_profiles.grid(True)
ax_profiles.set_title("Instantaneous vs Running-Average Reflectivity Profiles")

# Instantaneous difference plot (static line)
ax_inst_diff.plot(time_labels, rain_diff_per_time, color="blue", lw=2, label="Instantaneous Diff")
current_dot_inst, = ax_inst_diff.plot([], [], 'ro', markersize=8)
ax_inst_diff.set_xlim(min(time_labels), max(time_labels))
ax_inst_diff.set_ylim(np.min(rain_diff_per_time) * 1.05, np.max(rain_diff_per_time) * 1.05)
ax_inst_diff.set_xlabel("Time (minutes)")
ax_inst_diff.set_ylabel("Rain Rate Diff (mm/h)")
ax_inst_diff.grid(True)
ax_inst_diff.set_title("Instantaneous Rain Rate Difference at lowest altitude")
ax_inst_diff.legend()


# Cumulative difference plot
#ax_cum_diff.plot(time_labels, cumulative_rain_diff, color="gray", lw=2, alpha=0.7)
ax_cum_diff.plot(time_labels, abs(rainfall_acc_error), color="gray", lw=2, alpha=0.7)
current_dot_cum, = ax_cum_diff.plot([], [], 'ro', markersize=8)
ax_cum_diff.set_xlim(min(time_labels), max(time_labels))
#ax_cum_diff.set_ylim(0, np.max(cumulative_rain_diff) * 1.05)
#ax_cum_diff.set_ylim(np.min(rainfall_acc_error) * 1.05, np.max(rainfall_acc_error) * 1.05)
ax_cum_diff.set_ylim(0, np.max(rainfall_acc_error) * 1.05)
ax_cum_diff.set_xlabel("Time (minutes)")
#ax_cum_diff.set_ylabel("Rain Rate Diff (mm/h)")
ax_cum_diff.set_ylabel("mm/m^2")
ax_cum_diff.grid(True)
ax_cum_diff.set_title("Difference of rainfall accumulation over time")


# -------------------------
# Update function
# -------------------------
def update(frame):
    # Update profile lines
    line_reg.set_xdata(reflect_regular[:, frame])
    line_avg.set_xdata(reflect_avg[:, frame])
    
    # Update moving dot on cumulative plot
   # current_dot_cum.set_data([time_labels[frame]], [cumulative_rain_diff[frame]])
    current_dot_cum.set_data([time_labels[frame]], [abs(rainfall_acc_error[frame])])
    
    # Update moving dot on instantaneous plot
    current_dot_inst.set_data([time_labels[frame]], [rain_diff_per_time[frame]])
    
    return line_reg, line_avg, current_dot_cum, current_dot_inst

ani = FuncAnimation(fig, update, frames=n_times, interval=200, blit=True)
plt.tight_layout()
# Save as MP4 (requires ffmpeg installed)
ani.save('my_animation.gif', writer='pillow', fps=5)

plt.show()
