#!/usr/bin/env python3

import argparse
import json
import os
from pathlib import Path
from PIL import Image
from utils.imagepipe import write_c_bytes, write_full_image


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input", help="Input font")
    parser.add_argument("-o", action="store", required=True, dest="output", help="Output C header")
    params = parser.parse_args()

    src = Path(params.input).resolve()
    dst = Path(params.output).resolve()
    os.makedirs(dst.parent, exist_ok=True)
    convert_font(src, dst)


def convert_font(json_path: Path, dst_path: Path):
    with open(json_path, "r") as fp:
        info = json.load(fp)["font"]

    bmp_path = json_path.parent / info["file"]

    return convert_font_c(bmp_path, dst_path, info["char_width"], info["char_height"])


def convert_font_c(bmp_file: Path, c_file: Path, icon_w: int, icon_h: int):
    with open(bmp_file, "rb") as fp:
        with Image.open(fp) as image:
            binary = convert_font_binary(image, icon_w, icon_h)
            columns = image.width
            bytes_per_row = (image.height + 7) // 8

    with open(c_file, "w") as fp:
        print(f"const unsigned char {c_file.stem}_bits[] = {{", file=fp)
        write_c_bytes(fp, binary, 0, 8)
        for start in range(0, bytes_per_row * columns, columns):
            write_c_bytes(fp, binary, start + 8, columns)
        print("};", file=fp)
        print(f"const FONT *{c_file.stem} = (const FONT*) &{c_file.stem}_bits;", file=fp)


def convert_font_binary(src: Image.Image, icon_w: int, icon_h: int):
    icons_per_width = src.width // icon_w
    icons_per_height = src.height // icon_h
    if src.width % icon_w != 0:
        raise ValueError(f"Trailing font space: atlas width {src.width}, icon width {icon_w}")
    if src.height % icon_h != 0:
        raise ValueError(f"Trailing font space: atlas height {src.width}, icon height {icon_w}")
    if icons_per_width <= 0:
        raise ValueError(f"Atlas too short in X direction")
    if icons_per_height <= 0:
        raise ValueError(f"Atlas too short in Y direction")
    return write_full_image(src, 0x0300, icons_per_width, icons_per_height, icon_w, icon_h)


if __name__ == '__main__':
    main()
