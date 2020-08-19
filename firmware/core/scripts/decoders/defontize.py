#!/usr/bin/env python3

import argparse
import json
import os
import struct
from pathlib import Path
from PIL import Image


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input", help="Input LMS font file")
    parser.add_argument("-o", action="store", required=True, dest="output", help="Output JSON font file")
    params = parser.parse_args()

    src = Path(params.input).resolve()
    dst = Path(params.output).resolve()
    os.makedirs(dst.parent, exist_ok=True)
    defontize(src, dst)


def defontize(src_binary: Path, dst_json: Path):
    with open(src_binary, "rb") as fp:
        raw = fp.read()

    fmt, byte_count, x_count, y_count, char_w, char_h = struct.unpack_from(">HHBBBB", raw, 0)
    if fmt != 0x0300:
        raise ValueError(f"Illegal format: {hex(fmt)}")
    if (char_h % 8) != 0:
        raise ValueError(f"Icon height not divisible by eight: {char_h}")
    expect_bytes = x_count * y_count * char_w * char_h // 8
    if byte_count < expect_bytes:
        raise ValueError(f"Unexpected data size: expected {expect_bytes} != real {byte_count}")

    item = {
        "font": {
            "char_width": char_w,
            "char_height": char_h,
            "file": f"{dst_json.stem}.png"
        }
    }

    with open(dst_json.parent / item["font"]["file"], "wb") as fp:
        with Image.new("P", (x_count * char_w, y_count * char_h), 0) as img:
            img.putpalette([255, 255, 255, 0, 0, 0])
            defontize_one(raw, img)
            img.save(fp, "PNG")

    with open(dst_json, "w") as fp:
        json.dump(item, fp, indent=2)


def defontize_one(raw: bytes, img: Image.Image):
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
