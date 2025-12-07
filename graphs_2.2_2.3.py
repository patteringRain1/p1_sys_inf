import pandas as pd
import matplotlib.pyplot as plt

csv_files = {
    "Test & Set": "test_and_set.csv",
    "Test & Test & Set": "test_and_test_and_set.csv"
}

version_labels = list(csv_files.keys())
colors = ["#4A90E2", "#50E3C2"]  

df_first = pd.read_csv(list(csv_files.values())[0])
threads = sorted(df_first["threads"].unique())
n_threads = len(threads)
n_versions = len(csv_files)

width = 0.28
positions = []
all_box_data = []

for label, csv_file in csv_files.items():
    df = pd.read_csv(csv_file)
    version_data = [df[df["threads"] == t]["time"].values for t in threads]
    all_box_data.append(version_data)

flat_data = []
for t_idx in range(n_threads):
    for v_idx in range(n_versions):
        flat_data.append(all_box_data[v_idx][t_idx])
        pos = t_idx + 1 + (v_idx - n_versions/2 + 0.5) * width
        positions.append(pos)

plt.figure(figsize=(12, 7))

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

for i, patch in enumerate(bplots["boxes"]):
    patch.set_facecolor(colors[i % n_versions])
    patch.set_edgecolor("black")

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

plt.xticks(range(1, n_threads + 1), threads, fontsize=12)
plt.yticks(fontsize=12)
plt.xlabel("Nombre de threads", fontsize=14)
plt.ylabel("Temps (s)", fontsize=14)
plt.title("Test & Set vs Test & Test & Set", fontsize=16, weight="bold")

plt.grid(axis='y', linestyle='--', alpha=0.5)

plt.legend(fontsize=12, frameon=True, loc="upper left")
plt.tight_layout()

plt.savefig("test_set_vs_test_test_set.pdf")
plt.close()
