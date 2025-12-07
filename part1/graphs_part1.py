import pandas as pd
import matplotlib.pyplot as plt
import re

programs = {
    "Philosophes": "measures_philosophes.csv",
    "Lecteurs / Ecrivains": "measures_rw.csv",
    "Producteurs / Consommateurs": "measures_producteurs_consommateurs.csv"
}

for name, csv_file in programs.items():
    df = pd.read_csv(csv_file)

    plt.figure(figsize=(8,6))
    df.boxplot(column="time", by="threads")
    plt.title(f"{name}")
    plt.suptitle("")  
    plt.xlabel("Number of threads")
    plt.ylabel("Time (s)")
    plt.ylim(bottom=0)
    plt.tight_layout()
    plt.savefig(f"{safe_filename(name)}_boxplot.pdf")
    plt.show()
    plt.close()

plt.figure(figsize=(8,6))
for name, csv_file in programs.items():
    df = pd.read_csv(csv_file)

    stats = df.groupby("threads")["time"]
    plt.errorbar(stats.mean().index, stats.mean().values,
                 yerr=stats.std().values, fmt='-o', capsize=5, label=name)

plt.title("Comparison of execution times for 3 programs")
plt.xlabel("Number of threads")
plt.ylabel("Time (s)")
plt.ylim(bottom=0)
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.savefig("combined_boxplot.pdf")
plt.show()
