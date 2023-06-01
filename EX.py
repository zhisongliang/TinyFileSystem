#!/usr/bin/env python3
import argparse


def read_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description='A scheduling simulation script.')
    parser.add_argument(
        'input_file', help='Path to the input file containing job information')
    parser.add_argument(
        '-p', '--algorithm', help='Scheduling algorithm to use')
    # only used for round robin scheduling
    parser.add_argument(
        '-q', '--quantum', type=int, help='Time quantum for round robin scheduling')

    args = parser.parse_args()
    return args


if __name__ == '__main__':
    args = read_args()
    print(args)
