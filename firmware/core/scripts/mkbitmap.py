#!/usr/bin/env python3

import argparse
import json
import os
from pathlib import Path
from PIL import Image
from utils.imagepipe import write_c_bytes, write_full_image


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input", help="Input bitmap")
    parser.add_argument("-o", action="store", required=True, dest="output", help="Output C header")
    params = parser.parse_args()

    src = Path(params.input).resolve()
    dst = Path(params.output).resolve()
    convert_bitmap(src, dst)


def convert_bitmap(json_path: Path, dst_path: Path):
    with open(json_path, "r") as fp:
        info = json.load(fp)["bitmap"]

    bmp_path = json_path.parent / info["file"]

    return convert_bitmap_c(bmp_path, dst_path, info["start_x"], info["start_y"])


def convert_bitmap_c(bmp_file: Path, c_file: Path, start_x: int, start_y: int):
    with open(bmp_file, "rb") as fp:
        with Image.open(fp) as image:
            binary = convert_bitmap_binary(image, start_x, start_y)
            columns = image.width
            bytes_per_row = (image.height + 7) // 8

    os.makedirs(c_file.parent, exist_ok=True)
    with open(c_file, "w") as fp:
        print(f"const unsigned char {c_file.stem}_bits[] = {{", file=fp)
        write_c_bytes(fp, binary, 0, 8)
        for start in range(0, bytes_per_row * columns, columns):
            write_c_bytes(fp, binary, start + 8, columns)
        print("};", file=fp)
        print(f"const BMPMAP *{c_file.stem} = (const BMPMAP*) &{c_file.stem}_bits;", file=fp)
        print(f"const uint16_t {c_file.stem}_size = sizeof({c_file.stem}_bits);", file=fp)


def convert_bitmap_binary(src: Image.Image, start_x: int, start_y: int):
    if start_x < 0 or start_x >= 256:
        raise ValueError(f"Start X coordinate is invalid: {start_x}")
    if start_y < 0 or start_y >= 256:
        raise ValueError(f"Start Y coordinate is invalid: {start_x}")

    return write_full_image(src, 0x0200, start_x, start_y, src.width, src.height)


if __name__ == '__main__':
    main()
