#!/usr/bin/env python3
import argparse
import json
import os
import typing
from pathlib import Path
from PIL import Image
from utils.imagepipe import write_image_header, write_image_data, write_c_bytes


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input", help="Input icon descriptor")
    parser.add_argument("-o", action="store", required=True, dest="output", help="Output C header")
    params = parser.parse_args()

    src = Path(params.input).resolve()
    dst = Path(params.output).resolve()
    os.makedirs(dst.parent, exist_ok=True)
    convert_icons_c(src, dst)


def convert_icons_c(json_path: Path, c_file: Path):
    with open(json_path, "r") as fp:
        info = json.load(fp)["icon"]

    raw_bytes, width, height = convert_icons_binary(json_path, info)

    with open(c_file, "w") as fp:
        print_icon_enum(info, fp)

        print(f"const unsigned char {c_file.stem}_bits[] = {{", file=fp)
        write_c_bytes(fp, raw_bytes, 0, 8)
        for start in range(0, len(raw_bytes) - 8, width):
            write_c_bytes(fp, raw_bytes, start + 8, width)
        print("};", file=fp)
        print(f"const ICON *{c_file.stem} = (const ICON*) &{c_file.stem}_bits;", file=fp)


def print_icon_enum(info: typing.Any,
                    fp: typing.IO):
    if info.get("enum", False):
        name = ""
        if "enum_name" in info:
            name = f" {info['enum_name']}"

        offset = 0
        print(f"enum{name} {{", file=fp)

        if "enum_empty" in info:
            print(f"  {info['enum_empty']} = 0,", file=fp)
            offset = 1
        for item in info['items']:
            if "macro" in item:
                print(f"  {item['macro']} = {item['index'] + offset},", file=fp)

        print(f"}};", file=fp)
        print("", file=fp)


def convert_icons_binary(json_path: Path, info: typing.Any):

    min_index = min(item["index"] for item in info["items"])
    max_index = max(item["index"] for item in info["items"])
    if min_index < 0:
        raise ValueError(f"Icon index {min_index} is invalid.")
    atlas_count = max_index + 1
    icon_w = info["icon_width"]
    icon_h = info["icon_height"]
    if (icon_h % 8) != 0:
        raise ValueError(f"Icon height is not a multiple of 8: {icon_h}")
    x_count = 1
    y_count = atlas_count
    x_size = x_count * icon_w
    y_size = y_count * icon_h
    raw = bytearray(8 + x_size * y_size // 8)

    write_image_header(raw, 0x0400, x_size, y_size, x_count, y_count, icon_w, icon_h)

    for item in info["items"]:
        if "file" not in item:
            continue

        start = 8 + icon_w * icon_h // 8 * item["index"]

        with open(json_path.parent / item["file"], "rb") as fp:
            with Image.open(fp) as image:
                if image.width != icon_w:
                    raise ValueError(f"Image {item['file']} has illegal width {image.width}")
                if image.height != icon_h:
                    raise ValueError(f"Image {item['file']} has illegal height {image.height}")
                write_image_data(raw, image, start)

    return raw, x_size, y_size


if __name__ == '__main__':
    main()
