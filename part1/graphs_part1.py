"""
Script de visualisation des résultats expérimentaux pour trois programmes
concurrents : Philosophes, Lecteurs/Écrivains, Producteurs/Consommateurs.

Fonctionnement :
    - Charge les fichiers CSV contenant les temps d'exécution associés à
      différents nombres de threads.
    - Produit un boxplot par programme permettant d’observer la dispersion
      des mesures.
    - Génère un graphique combiné comparant les moyennes et écarts-types
      des trois programmes en fonction du nombre de threads.
    - Sauvegarde chaque figure au format PDF.

Entrées :
    - measures_philosophes.csv
    - measures_rw.csv
    - measures_producteurs_consommateurs.csv

Sorties :
    - Philosophes_boxplot.pdf
    - Lecteurs_Écrivains_boxplot.pdf
    - Producteurs_-_Consommateurs_boxplot.pdf
    - combined_boxplot.pdf

Prérequis :
    - Python 3.x
    - pandas
    - matplotlib.pyplot
"""

import pandas as pd
import matplotlib.pyplot as plt
import re

# Rend un nom lisible compatible en nom de fichier
def safe_filename(name: str) -> str:
    return re.sub(r'[^a-zA-Z0-9_-]+', '_', name)

# CSV associés à chaque programme
programs = {
    "Philosophes": "measures_philosophes.csv",
    "Lecteurs / Ecrivains": "measures_rw.csv",
    "Producteurs / Consommateurs": "measures_producteurs_consommateurs.csv"
}

# === Graphique individuel en boxplot ===
for name, csv_file in programs.items():
    df = pd.read_csv(csv_file)

    plt.figure(figsize=(8, 6))
    df.boxplot(column="time", by="threads")  # boxplot groupé par threads

    plt.title(name)
    plt.suptitle("")  # supprime le titre auto ajouté par pandas
    plt.xlabel("Number of threads")
    plt.ylabel("Time (s)")
    plt.ylim(bottom=0)
    plt.tight_layout()

    plt.savefig(f"{safe_filename(name)}_boxplot.pdf")
    plt.show()
    plt.close()

# === Graphique combiné (moyenne + écart-type) ===
plt.figure(figsize=(8, 6))

for name, csv_file in programs.items():
    df = pd.read_csv(csv_file)

    stats = df.groupby("threads")["time"]

    # Courbe des moyennes avec barres d’erreur pour les écarts-types
    plt.errorbar(
        stats.mean().index,
        stats.mean().values,
        yerr=stats.std().values,
        fmt='-o',
        capsize=5,
        label=name
    )

plt.title("Comparison of execution times for 3 programs")
plt.xlabel("Number of threads")
plt.ylabel("Time (s)")
plt.ylim(bottom=0)
plt.grid(True)
plt.legend()
plt.tight_layout()

plt.savefig("combined_boxplot.pdf")
plt.show()
