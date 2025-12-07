import pandas as pd
import matplotlib.pyplot as plt

program_versions = {
    "Lecteurs / Ecrivains": [
        "measures_rw.csv",
        "Lecteurs_Ecrivains_test.csv",
        "Lecteurs_Ecrivains_test_set.csv"
    ],
    "Philosophes": [
        "measures_philosophes.csv",
        "Philosophes_test.csv",
        "Philosophes_test_set.csv"
    ],
    "Producteurs / Consommateurs": [
        "measures_prod_cons.csv",
        "Prod_Cons_test.csv",
        "Prod_Cons_test_set.csv"
    ]
}

version_labels = ["Test", "Test & Set", "Test & Test & Set"]
colors = ['skyblue', 'lightgreen', 'lightcoral']

for program, csv_files in program_versions.items():
    plt.figure(figsize=(10,6))

    df_first = pd.read_csv(csv_files[0])
    threads = sorted(df_first["threads"].unique())
    n_threads = len(threads)
    n_versions = len(csv_files)

    width = 0.2
    positions = []
    all_box_data = []

    for i, csv_file in enumerate(csv_files):
        df = pd.read_csv(csv_file)
        version_data = [df[df["threads"] == t]["time"].values for t in threads]
        all_box_data.append(version_data)

    flat_data = []
    for t_idx in range(n_threads):
        for v_idx in range(n_versions):
            flat_data.append(all_box_data[v_idx][t_idx])
            pos = t_idx + 1 + (v_idx - n_versions/2 + 0.5) * width
            positions.append(pos)

    bplots = plt.boxplot(flat_data, positions=positions, widths=width, patch_artist=True)

    for i, patch in enumerate(bplots['boxes']):
        patch.set_facecolor(colors[i % n_versions])

    for v_idx, version_data in enumerate(all_box_data):
        means = [data.mean() for data in version_data]
        line_pos = [t + 1 + (v_idx - n_versions/2 + 0.5) * width for t in range(n_threads)]
        plt.plot(line_pos, means, color=colors[v_idx], marker='o', linestyle='-', linewidth=2, label=version_labels[v_idx])

    plt.xticks(range(1, n_threads+1), threads)
    plt.xlabel("Nombre de threads")
    plt.ylabel("Temps (s)")
    plt.title(f"{program}")
    plt.ylim(bottom=0)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()

    safe_name = program.replace(" ", "_").replace("/", "-")
    plt.savefig(f"{safe_name}.pdf")
    plt.close()
