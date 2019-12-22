#!/usr/bin/env python3

"""
Driver script for running mapwords benchmark and
generating performance analysis results.
"""

import argparse
import glob
import itertools
import platform
import re
import subprocess
from argparse import Namespace
from dataclasses import dataclass
from typing import Sequence

import matplotlib.pyplot as plt
import numpy as np

STATS_PATTERN = re.compile(r"stats:\s(.*)=(.*)")


@dataclass
class Stats(object):
    word_count: int = 0
    collisions: int = 0
    map_size: int = 0
    char_count: int = 0
    duration: float = 0
    hashf: str = ""

    def __post_init__(self):
        self.word_count = int(self.word_count)
        self.collisions = int(self.collisions)
        self.map_size = int(self.map_size)
        self.char_count = int(self.char_count)
        self.duration = float(self.duration)


def parse_args() -> Namespace:
    ap = argparse.ArgumentParser()
    ap.add_argument("mapwords", help="path to mapwords")
    ap.add_argument("-f", "--file", help="path to text file", nargs="+")

    args = ap.parse_args()
    if platform.system() == "Windows":
        args.file = itertools.chain(*[glob.glob(f) for f in args.file])

    return args


def plot_complexities(stats: Sequence[Stats], elements: int):
    x = np.array(range(1, elements), dtype=np.uint64)
    fig, axs = plt.subplots(nrows=1, ncols=2)
    axs[0].plot(x, np.log2(x), label="O(log(n))")
    axs[0].plot(x, x * np.log(x), label="O(n log(n))")
    axs[0].plot(x, x, label="O(n)")
    axs[0].hlines(1, xmin=1, xmax=elements, label="O(1)")

    data = {}
    for stat in stats:
        data[stat.hashf] = {}
        data[stat.hashf]["s_x"] = []
        data[stat.hashf]["s_y"] = []

    wps = []
    for stat in stats:
        data[stat.hashf]["s_x"].append(stat.word_count)
        data[stat.hashf]["s_y"].append(stat.duration)
        w = stat.word_count / stat.duration
        wps.append(w)
        print(f"{stat.hashf}: {w} words per second")

    for hashf in data:
        data[hashf]["s_x"].sort()
        data[hashf]["s_y"].sort()

        axs[1].plot(
            data[hashf]["s_x"],
            data[hashf]["s_y"],
            label=hashf,
        )

    print(f"words per second: min={min(wps)}, "
          f"max={max(wps)}, avg={sum(wps) / len(wps)}")

    axs[1].set_xlabel("word count")
    axs[1].set_ylabel("duration (s)")

    axs[0].set_xlabel("elements")
    axs[0].set_ylabel("operations")

    axs[0].legend()
    axs[1].legend()


def main():
    args = parse_args()

    hash_functions = [
        "hash_djb2",
        "hash_sdbm",
        "hash_java",
    ]

    outputs = []
    for f in args.file:
        for hf in hash_functions:
            print(f"analyzing: '{f}' ({hf})")
            out = subprocess.check_output([args.mapwords, "-f", f, "-h", hf])
            outputs.append(out.decode("utf-8"))

    stats = []
    for o in outputs:
        d = {}
        for match in STATS_PATTERN.finditer(o):
            d[match.groups()[0].strip()] = match.groups()[1].strip()
        s = Stats(**d)
        stats.append(s)

    plot_complexities(stats=stats, elements=1000)

    plt.show()


if __name__ == '__main__':
    main()
