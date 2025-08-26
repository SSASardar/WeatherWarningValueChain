import os
import numpy as np
import matplotlib.pyplot as plt

# File paths
input_file = 'outputs/stats.txt'
output_file = 'plots/scan_metrics_plot.png'

# Ensure the output directory exists
os.makedirs(os.path.dirname(output_file), exist_ok=True)

# Load data: assuming space-delimited with header
data = np.loadtxt(input_file, skiprows=1)

# Extract columns
scan_id = data[:, 0]
MSE = data[:, 1]
MAE = data[:, 2]
Bias = data[:, 3]
total_measured_mm2 = data[:, 6]
total_true_mm2 = data[:, 7]

# Compute time
time = scan_id * 5.0  # assuming 5 min intervals

# Create figure with 4 subplots (1 column, 4 rows)
fig, axes = plt.subplots(4, 1, figsize=(10, 16), sharex=True)

# Plot MSE
axes[0].plot(time, MSE, marker='o', color='blue', label='MSE')
axes[0].set_ylabel('MSE')
axes[0].set_title('Mean Squared Error over Time')
axes[0].grid(True)

# Plot MAE
axes[1].plot(time, MAE, marker='o', color='green', label='MAE')
axes[1].set_ylabel('MAE')
axes[1].set_title('Mean Absolute Error over Time')
axes[1].grid(True)

# Plot Bias
axes[2].plot(time, Bias, marker='o', color='red', label='Bias')
axes[2].set_ylabel('Bias')
axes[2].set_title('Bias over Time')
axes[2].grid(True)

# Plot area-corrected total rainfall
axes[3].plot(time, total_measured_mm2, marker='o', color='purple', label='Measured mm²')
axes[3].plot(time, total_true_mm2, marker='o', color='orange', label='True mm²')
axes[3].set_ylabel('Total Rainfall (mm²)')
axes[3].set_xlabel('Time (min)')
axes[3].set_title('Area-Corrected Total Rainfall over Time')
axes[3].grid(True)
axes[3].legend()

# Adjust layout
plt.tight_layout()

# Save the plot
plt.savefig(output_file)
print(f"Plot saved to {output_file}")
plt.close()

