#!/usr/bin/env python3

import argparse
import json
import os
import struct
from pathlib import Path
from PIL import Image


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input", help="Input icon file")
    parser.add_argument("-o", action="store", required=True, dest="output", help="Output JSON descriptor")
    params = parser.parse_args()

    src = Path(params.input).resolve()
    dst = Path(params.output).resolve()
    os.makedirs(dst.parent, exist_ok=True)
    deiconize(src, dst)


def deiconize(src_binary: Path, dst_json: Path):
    with open(src_binary, "rb") as fp:
        raw = fp.read()

    fmt, byte_count, x_count, y_count, icon_w, icon_h = struct.unpack_from(">HHBBBB", raw, 0)
    if fmt != 0x0400:
        raise ValueError(f"Illegal format: {hex(fmt)}")
    if (icon_h % 8) != 0:
        raise ValueError(f"Icon height not divisible by eight: {icon_h}")
    expect_bytes = x_count * y_count * icon_w * icon_h // 8
    if byte_count < expect_bytes:
        raise ValueError(f"Unexpected data size: expected {expect_bytes} != real {byte_count}")

    result = {
        "icon": {
            "icon_width": icon_w,
            "icon_height": icon_h,
            "items": []
        }
    }

    for y in range(0, y_count):
        for x in range(0, x_count):
            index = x + y * x_count
            item = {
                "id": f"{dst_json.stem}{index}",
                "index": index,
                "file": f"{dst_json.stem}{index}.png"
            }
            with open(dst_json.parent / item["file"], "wb") as fp:
                with Image.new("P", (icon_w, icon_h), 0) as img:
                    img.putpalette([255, 255, 255, 0, 0, 0])
                    deiconize_one(raw, img, x, y, x_count)
                    img.save(fp, "PNG")
            result["icon"]["items"].append(item)

    with open(dst_json, "w") as fp:
        json.dump(result, fp, indent=2)


def deiconize_one(raw: bytes,
                  img: Image.Image,
                  x: int,
                  y: int,
                  x_count: int):
    px = img.load()
    single_icon_bytes = img.width * img.height // 8

    base = 8 + y * x_count * single_icon_bytes + x * img.width

    for iy_base in range(0, img.height, 8):
        for ix in range(0, img.width):
            addr = base + ix
            value = raw[addr]
            for iy_adjust in range(0, 8):
                iy = iy_base + iy_adjust
                is_black = (value & (1 << iy_adjust)) != 0
                px[ix, iy] = 1 if is_black else 0
        base += x_count * img.width


if __name__ == '__main__':
    main()
