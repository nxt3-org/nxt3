#!/usr/bin/env python3

import argparse
import json
import os
import struct
from pathlib import Path
from PIL import Image


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input", help="Input LMS icon file")
    parser.add_argument("-o", action="store", required=True, dest="output", help="Output JSON icon file")
    params = parser.parse_args()

    src = Path(params.input).resolve()
    dst = Path(params.output).resolve()
    os.makedirs(dst.parent, exist_ok=True)
    debitmapize(src, dst)


def debitmapize(src_binary: Path, dst_json: Path):
    with open(src_binary, "rb") as fp:
        raw = fp.read()

    fmt, byte_count, start_x, start_y, bmp_w, bmp_h = struct.unpack_from(">HHBBBB", raw, 0)
    if fmt != 0x0200:
        raise ValueError(f"Illegal format: {hex(fmt)}")
    expect_bytes = bmp_w * ((bmp_h + 7) // 8)
    if byte_count < expect_bytes:
        raise ValueError(f"Unexpected data size: expected {expect_bytes} != real {byte_count}")

    item = {
        "bitmap": {
            "start_x": start_x,
            "start_y": start_y,
            "file": f"{dst_json.stem}.png"
        }
    }

    with open(dst_json.parent / item["bitmap"]["file"], "wb") as fp:
        with Image.new("P", (bmp_w, bmp_h), 0) as img:
            img.putpalette([255, 255, 255, 0, 0, 0])
            debitmapize_one(raw, img)
            img.save(fp, "PNG")

    with open(dst_json, "w") as fp:
        json.dump(item, fp, indent=2)


def debitmapize_one(raw: bytes, img: Image.Image):
    px = img.load()
    base = 8

    for iy_base in range(0, img.height, 8):
        for ix in range(0, img.width):
            addr = base + ix
            value = raw[addr]
            for iy_adjust in range(0, 8):
                iy = iy_base + iy_adjust
                is_black = (value & (1 << iy_adjust)) != 0
                px[ix, iy] = 1 if is_black else 0
        base += img.width


if __name__ == '__main__':
    main()
