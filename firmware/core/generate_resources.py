#!/usr/bin/env python3
import json
import os
import sys
import typing
from colorama import init, Fore, Style
from PIL import Image
import struct

FILEFORMAT_BITMAP = 0x0200


def main():
    init()
    base = 1
    while base < len(sys.argv):
        op = sys.argv[base + 0]
        src = os.path.realpath(sys.argv[base + 1])
        dst = os.path.realpath(sys.argv[base + 2])
        extras = 0

        if op == 'bitmap':
            extras = 2
            start_x = int(sys.argv[base + 3])
            start_y = int(sys.argv[base + 4])
            convert_bitmap_c(src, dst, start_x, start_y)
        elif op == 'texts':
            extras = 0
            convert_texts(src, dst)

        base += 3 + extras


def convert_bitmap_c(bmp_file: str, c_file: str, start_x: int, start_y: int):
    name_ext = os.path.basename(c_file)
    name, _ = os.path.splitext(name_ext)

    print(Fore.LIGHTBLUE_EX + f"Compiling resource {name_ext}" + Style.RESET_ALL)

    with open(bmp_file, "rb") as fp:
        with Image.open(fp) as image:
            raw_bytes = convert_bitmap_binary(image, start_x, start_y)
            columns = image.width
            row_bytes = (image.height + 7) // 8

    os.makedirs(os.path.dirname(c_file), exist_ok=True)
    with open(c_file, "w") as out:
        out.write(f"const unsigned char {name}_bits[] = {{")
        write_c_bytes(out, raw_bytes, 0, 8)
        for start in range(0, row_bytes * columns, columns):
            write_c_bytes(out, raw_bytes, start + 8, columns)
        out.write("};\n")
        out.write(f"#define {name} ((const BMPMAP*) &{name}_bits)\n")
        out.write(f"#define {name}_size (sizeof({name}_bits))\n")


def write_c_bytes(out: typing.IO, array: bytearray, start: int, count: int):
    for byte in array[start:start + count]:
        out.write(str(byte))
        out.write(",")
    out.write("\n")


def convert_bitmap_binary(src: Image.Image, start_x: int, start_y: int):
    if start_x < 0 or start_x >= 256:
        raise ValueError(f"Start X coordinate is invalid: {start_x}")
    if start_y < 0 or start_y >= 256:
        raise ValueError(f"Start Y coordinate is invalid: {start_x}")

    px = src.load()

    header_bytes = 8
    width_bytes = src.width
    height_bytes = (src.height + 7) // 8
    buffer = bytearray(header_bytes + width_bytes * height_bytes)
    struct.pack_into(
        ">HHBBBB", buffer, 0,
        FILEFORMAT_BITMAP,
        width_bytes * height_bytes,
        start_x,
        start_y,
        src.width,
        src.height
    )
    index = header_bytes

    for y_base in range(0, src.height, 8):
        y_max = src.height - y_base
        if y_max >= 8:
            y_max = 8
        for x in range(0, width_bytes):
            pixel_byte = 0
            for y_adj in range(0, y_max):
                y = y_base + y_adj
                if px[x, y] < 127:
                    pixel_byte |= 1 << y_adj
            buffer[index] = pixel_byte
            index += 1
    return buffer


def convert_texts(src: str, dst: str):
    with open(src, "r") as fp:
        strings = json.load(fp)["strings"]

    with open(dst, "w") as fp:
        fp.write("// strings indices\n")
        fp.write("enum {\n")
        for item in strings:
            fp.write(f"    {item['name']},\n")
        fp.write(f"    TXT_STRING_COUNT,\n")
        fp.write("};\n\n")
        fp.write("// string values\n")
        fp.write("const char *UiStrings[] = {\n")
        for item in strings:
            fp.write(f"    [{item['name']}] = \"{item['value']}\",\n")
        fp.write("};\n")


if __name__ == '__main__':
    main()
