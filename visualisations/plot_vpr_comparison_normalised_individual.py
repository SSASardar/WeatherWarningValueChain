import numpy as np
import matplotlib.pyplot as plt

# Path to the file
file_path = "outputs/vpr_comparison.txt"

# Load the data, skipping the header row
data = np.loadtxt(file_path, skiprows=1)

# Columns: 0=Height, 1=VPR_strat, 2=VPR_A_gmd, 3=VPR_A_d
height = data[:, 0]
vpr_a_clima = data[:, 1]
vpr_a_gmd = data[:, 2]
vpr_a_d = data[:, 3]

def normalize_profile(profile, height):
    """
    Normalize non-zero values of a profile and its corresponding heights.
    Returns normalized profile and normalized heights.
    """
    mask = profile != 0
    profile_nonzero = profile[mask]
    height_nonzero = height[mask]
    
    # Min-max normalization
    profile_norm = (profile_nonzero - np.min(profile_nonzero)) / (np.max(profile_nonzero) - np.min(profile_nonzero))
    height_norm = (height_nonzero - np.min(height_nonzero)) / (np.max(height_nonzero) - np.min(height_nonzero))
    
    return profile_norm, height_norm

# Normalize each VPR independently
vpr_a_clima_norm, height_a_clima_norm = normalize_profile(vpr_a_clima, height)
vpr_a_gmd_norm, height_gmd_norm = normalize_profile(vpr_a_gmd, height)
vpr_a_d_norm, height_d_norm = normalize_profile(vpr_a_d, height)

# Plot normalized profiles
plt.figure(figsize=(8, 10))
plt.plot(vpr_a_clima_norm, height_a_clima_norm, label='VPR_A_clima', color='blue')
#plt.plot(vpr_a_gmd_norm, height_gmd_norm, label='VPR_A_gmd', color='green')
#plt.plot(vpr_a_d_norm, height_d_norm, label='VPR_A_d', color='red')

# Labels and title
plt.xlabel('Normalized Reflectivity')
plt.ylabel('Normalized Height')
plt.title('Normalized Vertical Profile of Reflectivity "Climatological average"')
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()

