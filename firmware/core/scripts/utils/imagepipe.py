#!/usr/bin/env python3

import typing
from PIL import Image
import struct


def write_full_image(src: Image.Image, fmt: int, aux1: int, aux2: int, aux3: int, aux4: int):
    header_bytes = 8
    width_bytes = src.width
    height_bytes = (src.height + 7) // 8
    buffer = bytearray(header_bytes + width_bytes * height_bytes)

    write_image_header(buffer, fmt, src.width, src.height, aux1, aux2, aux3, aux4)
    write_image_data(buffer, src, 8)
    return buffer


def write_image_header(buffer: bytearray, fmt: int, width: int, height: int,
                       aux1: int, aux2: int, aux3: int, aux4: int):
    struct.pack_into(
        ">HHBBBB", buffer, 0,
        fmt,
        width * (height + 7) // 8,
        aux1,
        aux2,
        aux3,
        aux4
    )


def write_image_data(buffer: bytearray,
                     src: Image.Image,
                     start: int):
    index = start
    px = src.load()
    for y_base in range(0, src.height, 8):
        y_max = src.height - y_base
        if y_max >= 8:
            y_max = 8
        for x in range(0, src.width):
            pixel_byte = 0
            for y_adj in range(0, y_max):
                y = y_base + y_adj
                if px[x, y] != 0:
                    pixel_byte |= 1 << y_adj
            buffer[index] = pixel_byte
            index += 1


def write_c_bytes(out: typing.IO, array: bytearray, start: int, count: int):
    print("  ", end="", file=out)
    for byte in array[start:start + count]:
        print(f"{byte},", end="", file=out)
    print(file=out)
