"""
Script d’analyse et de visualisation des performances des verrous
Test-and-Set (TAS) et Test-and-Test-and-Set (TTAS).

Fonctionnement :
    - Charge deux fichiers CSV contenant les temps d’exécution pour
      différents nombres de threads.
    - Regroupe les données par version du verrou et par nombre de threads.
    - Génère un boxplot comparatif des deux techniques, accompagné
      d’une courbe représentant la moyenne de chaque groupe.
    - Enregistre la figure finale au format PDF.

Entrées :
    - test_and_set.csv
    - test_and_test_and_set.csv

Sortie :
    - test_set_vs_test_test_set.pdf : graphique comparatif

Prérequis :
    - Python 3.x
    - pandas, matplotlib
"""


import pandas as pd
import matplotlib.pyplot as plt

# CSV files to compare
csv_files = {
    "Test & Set": "test_and_set.csv",
    "Test & Test & Set": "test_and_test_and_set.csv"
}

version_labels = list(csv_files.keys())
colors = ["#4A90E2", "#50E3C2"]  # Colors for the two versions

# Load first CSV to extract thread values
df_first = pd.read_csv(list(csv_files.values())[0])
threads = sorted(df_first["threads"].unique())
n_threads = len(threads)
n_versions = len(csv_files)

width = 0.28  # Horizontal spacing between boxplots
positions = []
all_box_data = []

# Load all CSV data and regroup by version × threads
for label, csv_file in csv_files.items():
    df = pd.read_csv(csv_file)
    version_data = [df[df["threads"] == t]["time"].values for t in threads]
    all_box_data.append(version_data)

# Flatten + compute positions for matplotlib
flat_data = []
for t_idx in range(n_threads):
    for v_idx in range(n_versions):
        flat_data.append(all_box_data[v_idx][t_idx])
        pos = t_idx + 1 + (v_idx - n_versions/2 + 0.5) * width
        positions.append(pos)

plt.figure(figsize=(12, 7))

# Draw boxplots
bplots = plt.boxplot(
    flat_data,
    positions=positions,
    widths=width,
    patch_artist=True,
    medianprops=dict(color="black", linewidth=2),
    boxprops=dict(linewidth=1.5),
    whiskerprops=dict(linewidth=1.5),
    capprops=dict(linewidth=1.5)
)

# Apply colors to each box
for i, patch in enumerate(bplots["boxes"]):
    patch.set_facecolor(colors[i % n_versions])
    patch.set_edgecolor("black")

# Plot means as a line for each version
for v_idx, version_data in enumerate(all_box_data):
    means = [data.mean() for data in version_data]
    line_pos = [t + 1 + (v_idx - n_versions/2 + 0.5) * width for t in range(n_threads)]
    plt.plot(
        line_pos, means,
        color=colors[v_idx],
        marker='o',
        markersize=8,
        linewidth=2.5,
        label=version_labels[v_idx]
    )

# Axis labels & formatting
plt.xticks(range(1, n_threads + 1), threads, fontsize=12)
plt.yticks(fontsize=12)
plt.xlabel("Nombre de threads", fontsize=14)
plt.ylabel("Temps (s)", fontsize=14)
plt.title("Test & Set vs Test & Test & Set", fontsize=16, weight="bold")

plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.legend(fontsize=12, frameon=True, loc="upper left")
plt.tight_layout()

# Save graph
plt.savefig("test_set_vs_test_test_set.pdf")
plt.close()
