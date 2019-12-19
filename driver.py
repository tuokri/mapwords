#!/usr/bin/env python3

"""
Driver script for running mapwords benchmark and
generating performance analysis results.
"""

import subprocess
import argparse
from argparse import Namespace


def parse_args() -> Namespace:
    ap = argparse.ArgumentParser()
    ap.add_argument("mapwords", help="path to mapwords")
    ap.add_argument("-f", "--file", help="path to text file", nargs="?")
    return ap.parse_args()


def main():
    args = parse_args()
    subprocess.check_output(["mapwords", args.file])


if __name__ == '__main__':
    main()
