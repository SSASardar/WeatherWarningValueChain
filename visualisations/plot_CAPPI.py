import os
import re
import sys
import numpy as np
import matplotlib.pyplot as plt

def load_ppi_file(filename):
    """Load a PPI slice text file into two numpy arrays: reflectivity, height."""
    with open(filename, "r") as f:
        lines = f.readlines()

    # Skip comment lines starting with '#'
    data_lines = [line.strip() for line in lines if not line.startswith("#")]

    refl_list = []
    height_list = []

    for line in data_lines:
        row_refl = []
        row_h = []
        tokens = line.split()
        for i in range(0, len(tokens), 2):
            try:
                refl = float(tokens[i])
                h = float(tokens[i+1])
            except ValueError:
                refl = np.nan
                h = np.nan
            row_refl.append(refl)
            row_h.append(h)
        refl_list.append(row_refl)
        height_list.append(row_h)

    reflectivity = np.array(refl_list)
    height = np.array(height_list)

    return reflectivity, height


def plot_ppi(filename, outdir="plots"):
    """Plot reflectivity and height from a PPI file into two side-by-side plots."""
    reflectivity, height = load_ppi_file(filename)

    if not os.path.exists(outdir):
        os.makedirs(outdir)

    fig, axes = plt.subplots(1, 2, figsize=(12, 6))

    im0 = axes[0].imshow(reflectivity, cmap="viridis", origin="lower")
    axes[0].set_title("Reflectivity")
    plt.colorbar(im0, ax=axes[0], fraction=0.046, pad=0.04)

    im1 = axes[1].imshow(height, cmap="plasma", origin="lower")
    axes[1].set_title("Height")
    plt.colorbar(im1, ax=axes[1], fraction=0.046, pad=0.04)

    fig.suptitle(f"PPI Plot: {os.path.basename(filename)}", fontsize=14)

    outpath = os.path.join(outdir, os.path.basename(filename).replace(".txt", ".png"))
    plt.savefig(outpath, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved plot to {outpath}")


def process_all_ppis(indir="outputs", outdir="plots"):
    """Find all files named ppi*.txt in indir and plot them."""
    if not os.path.exists(indir):
        print(f"Input directory {indir} does not exist.")
        return

    files = sorted(
        [f for f in os.listdir(indir) if re.match(r"ppi\d+\.txt", f)]
    )

    if not files:
        print("No PPI files found.")
        return

    for f in files:
        infile = os.path.join(indir, f)
        plot_ppi(infile, outdir)


if __name__ == "__main__":
    # If user specifies a file, plot just that one.
    # If no argument, process all PPIs.
    if len(sys.argv) > 1:
        plot_ppi(sys.argv[1])
    else:
        process_all_ppis()

