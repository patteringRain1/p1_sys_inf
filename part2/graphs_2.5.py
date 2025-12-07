"""
Script de visualisation permettant de comparer les performances
de plusieurs versions d'implémentation de trois problèmes concurrents :

    - Lecteurs / Écrivains
    - Philosophes
    - Producteurs / Consommateurs

Fonctionnement :
    - Pour chaque problème, le script charge trois fichiers CSV :
          * Version simple (Test)
          * Test-and-Set (TAS)
          * Test-and-Test-and-Set (TTAS)
    - Les données sont regroupées par nombre de threads.
    - Un boxplot est généré pour comparer les distributions des temps
      d'exécution, accompagné d'une courbe représentant la moyenne par version.
    - Chaque graphique est enregistré au format PDF.

Entrées attendues :
    - measures_rw.csv, Lecteurs_Ecrivains_test.csv, Lecteurs_Ecrivains_test_set.csv
    - measures_philosophes.csv, Philosophes_test.csv, Philosophes_test_set.csv
    - measures_prod_cons.csv, Prod_Cons_test.csv, Prod_Cons_test_set.csv

Sorties :
    - Lecteurs_Écrivains.pdf
    - Philosophes.pdf
    - Producteurs_-_Consommateurs.pdf

Prérequis :
    - Python 3.x
    - pandas, matplotlib
"""

import pandas as pd
import matplotlib.pyplot as plt

# Fichiers CSV regroupés par programme
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

# Étiquettes de légende + couleurs
version_labels = ["Test", "Test & Set", "Test & Test & Set"]
colors = ['skyblue', 'lightgreen', 'lightcoral']

# Génération d’un graphique par type de programme
for program, csv_files in program_versions.items():
    plt.figure(figsize=(10, 6))

    # Extraction de la liste des threads depuis le premier CSV
    df_first = pd.read_csv(csv_files[0])
    threads = sorted(df_first["threads"].unique())
    n_threads = len(threads)
    n_versions = len(csv_files)

    width = 0.2  # espacement horizontal
    positions = []
    all_box_data = []

    # Chargement des données pour chaque version
    for i, csv_file in enumerate(csv_files):
        df = pd.read_csv(csv_file)
        version_data = [df[df["threads"] == t]["time"].values for t in threads]
        all_box_data.append(version_data)

    # Conversion en liste plate + positions des boxplots
    flat_data = []
    for t_idx in range(n_threads):
        for v_idx in range(n_versions):
            flat_data.append(all_box_data[v_idx][t_idx])
            pos = t_idx + 1 + (v_idx - n_versions/2 + 0.5) * width
            positions.append(pos)

    # Boxplot
    bplots = plt.boxplot(flat_data, positions=positions, widths=width, patch_artist=True)

    # Couleurs des boîtes
    for i, patch in enumerate(bplots["boxes"]):
        patch.set_facecolor(colors[i % n_versions])

    # Tracé des moyennes
    for v_idx, version_data in enumerate(all_box_data):
        means = [data.mean() for data in version_data]
        line_pos = [t + 1 + (v_idx - n_versions/2 + 0.5) * width for t in range(n_threads)]
        plt.plot(
            line_pos, means,
            color=colors[v_idx],
            marker='o',
            linestyle='-',
            linewidth=2,
            label=version_labels[v_idx]
        )

    # Configuration du graphique
    plt.xticks(range(1, n_threads + 1), threads)
    plt.xlabel("Nombre de threads")
    plt.ylabel("Temps (s)")
    plt.title(program)
    plt.ylim(bottom=0)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()

    # Enregistrement au format PDF
    safe_name = program.replace(" ", "_").replace("/", "-")
    plt.savefig(f"{safe_name}.pdf")
    plt.close()
