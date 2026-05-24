import csv
import os
import sys
from collections import defaultdict

if "MPLCONFIGDIR" not in os.environ:
    os.environ["MPLCONFIGDIR"] = os.path.join(os.getcwd(), ".mplcfg")
os.makedirs(os.environ["MPLCONFIGDIR"], exist_ok=True)
if "XDG_CACHE_HOME" not in os.environ:
    os.environ["XDG_CACHE_HOME"] = os.path.join(os.getcwd(), ".cache")
os.makedirs(os.environ["XDG_CACHE_HOME"], exist_ok=True)

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

plt.rcParams["font.family"] = "DejaVu Sans"

ALGOS = [
    "quick_std",
    "merge_std",
    "merge_lcp",
    "string_quick3",
    "msd_plain",
    "msd_switch",
]

DATA_TYPES = ["random", "reverse", "almost", "prefix"]
DATA_ORDER = ["random", "reverse", "almost", "prefix"]

ALGO_RU = {
    "quick_std": "Быстрая (стандартная)",
    "merge_std": "Слиянием (стандартная)",
    "merge_lcp": "String MergeSort (LCP)",
    "string_quick3": "Тернарная String QuickSort",
    "msd_plain": "MSD Radix (без переключ.)",
    "msd_switch": "MSD Radix (перекл. < 74)",
}

DATA_RU = {
    "random": "Случайные",
    "reverse": "Обратно отсортированные",
    "almost": "Почти отсортированные",
    "prefix": "С общими префиксами",
}

METRIC_RU = {
    "time_ns": "Время, нс",
    "char_cmp": "Посимвольные сравнения",
}

METRIC_FILE = {
    "time_ns": "time",
    "char_cmp": "cmp",
}

GRID_FILE = {
    "time_ns": "time.png",
    "char_cmp": "cmp.png",
}


def load_rows(path):
    rows = []
    with open(path, "r", encoding="utf-8") as f:
        rd = csv.DictReader(f)
        for r in rd:
            rows.append(
                {
                    "run": int(r["run"]),
                    "data_type": r["data_type"],
                    "n": int(r["n"]),
                    "algo": r["algo"],
                    "time_ns": int(r["time_ns"]),
                    "char_cmp": int(r["char_cmp"]),
                }
            )
    return rows


def aggregate_mean(rows):
    acc = defaultdict(lambda: [0, 0, 0])
    for r in rows:
        k = (r["data_type"], r["n"], r["algo"])
        acc[k][0] += r["time_ns"]
        acc[k][1] += r["char_cmp"]
        acc[k][2] += 1

    mean = {}
    for k, v in acc.items():
        s_t, s_c, c = v
        mean[k] = {
            "time_ns": s_t / c,
            "char_cmp": s_c / c,
            "cnt": c,
        }
    return mean


def get_data_types(rows):
    seen = set()
    for r in rows:
        seen.add(r["data_type"])
    data_types = []
    for dt in DATA_ORDER:
        if dt in seen:
            data_types.append(dt)
    for dt in sorted(seen):
        if dt not in data_types:
            data_types.append(dt)
    return data_types


def save_mean_csv(mean, out_csv):
    keys = sorted(mean.keys(), key=lambda x: (x[0], x[1], x[2]))
    with open(out_csv, "w", encoding="utf-8", newline="") as f:
        wr = csv.writer(f)
        wr.writerow(["data_type", "n", "algo", "mean_time_ns", "mean_char_cmp", "runs"])
        for dt, n, al in keys:
            wr.writerow([dt, n, al, f"{mean[(dt,n,al)]['time_ns']:.3f}", f"{mean[(dt,n,al)]['char_cmp']:.3f}", mean[(dt,n,al)]["cnt"]])


def plot_grid(mean, metric, out_png, data_types):
    cols = 2
    rows = (len(data_types) + cols - 1) // cols
    fig, axes = plt.subplots(rows, cols, figsize=(16, 5 * rows), sharex=True)
    if rows == 1 and cols == 1:
        axes = [axes]
    elif rows == 1:
        axes = list(axes)
    else:
        axes = axes.ravel()

    for idx, dt in enumerate(data_types):
        ax = axes[idx]
        for al in ALGOS:
            xs = []
            ys = []
            for (k_dt, k_n, k_al), vals in mean.items():
                if k_dt == dt and k_al == al:
                    xs.append(k_n)
                    ys.append(vals[metric])
            if not xs:
                continue
            pts = sorted(zip(xs, ys))
            xs = [p[0] for p in pts]
            ys = [p[1] for p in pts]
            ax.plot(xs, ys, marker="o", linewidth=1.7, markersize=3, label=ALGO_RU.get(al, al))

        ax.set_title(f"{DATA_RU.get(dt, dt)} | {METRIC_RU.get(metric, metric)}")
        ax.set_xlabel("Размер n")
        ax.set_ylabel(METRIC_RU.get(metric, metric))
        ax.grid(True, alpha=0.3)

    for idx in range(len(data_types), len(axes)):
        axes[idx].axis("off")

    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="upper center", ncol=3, frameon=False)
    fig.tight_layout(rect=[0, 0, 1, 0.93])
    fig.savefig(out_png, dpi=180)
    plt.close(fig)


def plot_per_type(mean, out_dir, data_types):
    for dt in data_types:
        for metric in ["time_ns", "char_cmp"]:
            fig, ax = plt.subplots(figsize=(11, 6))
            for al in ALGOS:
                xs = []
                ys = []
                for (k_dt, k_n, k_al), vals in mean.items():
                    if k_dt == dt and k_al == al:
                        xs.append(k_n)
                        ys.append(vals[metric])
                if not xs:
                    continue
                pts = sorted(zip(xs, ys))
                xs = [p[0] for p in pts]
                ys = [p[1] for p in pts]
                ax.plot(xs, ys, marker="o", linewidth=1.8, markersize=4, label=ALGO_RU.get(al, al))

            ax.set_title(f"{DATA_RU.get(dt, dt)} | {METRIC_RU.get(metric, metric)}")
            ax.set_xlabel("Размер n")
            ax.set_ylabel(METRIC_RU.get(metric, metric))
            ax.grid(True, alpha=0.3)
            ax.legend(loc="upper left", bbox_to_anchor=(1.02, 1), borderaxespad=0)
            fig.tight_layout()
            out_png = os.path.join(out_dir, f"{dt}_{METRIC_FILE[metric]}.png")
            fig.savefig(out_png, dpi=180, bbox_inches="tight")
            plt.close(fig)


def save_summary(mean, out_txt):
    ns = sorted({n for (_, n, _) in mean.keys()})
    if not ns:
        with open(out_txt, "w", encoding="utf-8") as f:
            f.write("")
        return
    n_last = ns[-1]
    lines = []

    lines.append("=== Лучшие алгоритмы при максимальном n ===")
    lines.append(f"n = {n_last}")

    dtypes = sorted({dt for (dt, _, _) in mean.keys()})
    for dt in dtypes:
        best_t = None
        best_c = None
        for al in ALGOS:
            k = (dt, n_last, al)
            if k not in mean:
                continue
            t = mean[k]["time_ns"]
            c = mean[k]["char_cmp"]
            if best_t is None or t < best_t[1]:
                best_t = (al, t)
            if best_c is None or c < best_c[1]:
                best_c = (al, c)

        if best_t is None:
            continue
        dt_ru = DATA_RU.get(dt, dt)
        bt = ALGO_RU.get(best_t[0], best_t[0])
        bc = ALGO_RU.get(best_c[0], best_c[0])
        lines.append(
            f"{dt_ru}: лучшее время = {bt} ({best_t[1]:.0f} нс), "
            f"меньше сравнений = {bc} ({best_c[1]:.0f})"
        )
    with open(out_txt, "w", encoding="utf-8") as f:
        for line in lines:
            f.write(line + "\n")


def main():
    if len(sys.argv) < 2:
        sys.exit(1)

    in_csv = sys.argv[1]
    out_dir = "set9/plots"
    if len(sys.argv) >= 3:
        out_dir = sys.argv[2]

    os.makedirs(out_dir, exist_ok=True)

    rows = load_rows(in_csv)
    mean = aggregate_mean(rows)
    data_types = get_data_types(rows)

    for dt in DATA_ORDER:
        old_names = [
            f"{dt}_time_ns.png",
            f"{dt}_char_cmp.png",
            f"{dt}_time.png",
            f"{dt}_cmp.png",
        ]
        for nm in old_names:
            old_png = os.path.join(out_dir, nm)
            if os.path.exists(old_png):
                os.remove(old_png)

    legacy = [
        "time_ns_grid.png",
        "char_cmp_grid.png",
    ]
    for nm in legacy:
        p = os.path.join(out_dir, nm)
        if os.path.exists(p):
            os.remove(p)

    save_mean_csv(mean, os.path.join(out_dir, "mean_results.csv"))

    plot_grid(mean, "time_ns", os.path.join(out_dir, GRID_FILE["time_ns"]), data_types)
    plot_grid(mean, "char_cmp", os.path.join(out_dir, GRID_FILE["char_cmp"]), data_types)
    plot_per_type(mean, out_dir, data_types)
    save_summary(mean, os.path.join(out_dir, "summary.txt"))


if __name__ == "__main__":
    main()
