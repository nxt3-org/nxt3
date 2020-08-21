#!/usr/bin/env python3

import argparse
import json
import os
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input", help="Input JSON file")
    parser.add_argument("-o", action="store", required=True, dest="output", help="Output C header")
    params = parser.parse_args()

    src = Path(params.input).resolve()
    dst = Path(params.output).resolve()
    os.makedirs(dst.parent, exist_ok=True)
    convert_texts(src, dst)


def convert_texts(src: Path, dst: Path):
    with open(src, "r") as fp:
        strings = json.load(fp)["strings"]

    with open(dst, "w") as fp:
        print("// strings indices", file=fp)
        print("enum {", file=fp)
        for item in strings:
            print(f"  {item['name']},", file=fp)
        print("  TXT_STRING_COUNT,", file=fp)
        print("};", file=fp)
        print("", file=fp)
        print("// string values", file=fp)
        print("const char *UiStrings[TXT_STRING_COUNT] = {", file=fp)
        for item in strings:
            print(f'  [{item["name"]}] = "{item["value"]}",', file=fp)
        print("};", file=fp)


if __name__ == '__main__':
    main()
