import numpy as np
import matplotlib.pyplot as plt

# Path to the file
file_path = "outputs/vpr_comparison.txt"

# Load the data, skipping the header row
data = np.loadtxt(file_path, skiprows=1)

# Columns: 0=Height, 1=VPR_strat, 2=VPR_A_gmd, 3=VPR_A_d
height = data[:, 0]
vpr_strat = data[:, 1]
vpr_a_gmd = data[:, 2]
vpr_a_d = data[:, 3]

# Filter out zero values for each profile
mask_strat = vpr_strat != 0
mask_gmd = vpr_a_gmd != 0
mask_d = vpr_a_d != 0

# Plot the three profiles
plt.figure(figsize=(8, 10))
plt.plot(vpr_strat[mask_strat], height[mask_strat], label='VPR_strat', color='blue')
plt.plot(vpr_a_gmd[mask_gmd], height[mask_gmd], label='VPR_A_gmd', color='green')
plt.plot(vpr_a_d[mask_d], height[mask_d], label='VPR_A_d', color='red')

# Labels and title
plt.xlabel('Reflectivity')
plt.ylabel('Height')
plt.title('Vertical Profile of Reflectivity (Non-zero values)')
plt.legend()
plt.grid(True)
#plt.gca().invert_yaxis()  # Optional: invert y-axis if needed

plt.tight_layout()
plt.show()

