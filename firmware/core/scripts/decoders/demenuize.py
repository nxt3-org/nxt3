#!/usr/bin/env python3

import argparse
import json
import struct
import typing
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", action="store", required=True, dest="input",
                        help="Menu binary")
    parser.add_argument("-m", action="store", required=True, dest="icons",
                        help="Icon definitions")
    parser.add_argument("-e", action="store", required=True, dest="enums",
                        help="Common enum definitions")
    parser.add_argument("-o", action="store", required=True, dest="output",
                        help="Menu definition")
    params = parser.parse_args()

    with open(Path(params.enums), "r") as fp:
        enum_db = json.load(fp)
    with open(Path(params.icons), "r") as fp:
        icon_db = json.load(fp)["icon"]
    with open(Path(params.input), "rb") as fp:
        binary = fp.read()
    obj = decompile_menus(binary, icon_db, enum_db)
    with open(Path(params.output), "w") as fp:
        json.dump(obj, fp, indent=2)


def decompile_menus(menu_binary: bytes,
                    icon_db: typing.Any,
                    enum_db: typing.Any):
    fmt, data_size, menu_size, menus, icon_w, icon_h = struct.unpack_from(">HHBBBB", menu_binary, 0)
    if fmt != 0x0700:
        raise ValueError(f"Invalid format: {hex(fmt)}")
    if data_size != (menus * menu_size):
        raise ValueError(f"Unexpected data size: {data_size}")
    if menu_size != 29:
        raise ValueError(f"Unexpected menu size: {menu_size}")

    result = {"menus": []}

    for start in range(8, 8 + data_size, menu_size):
        result["menus"].append(decompile_menu(menu_binary, start, icon_db, enum_db))
    return result


def decompile_menu(raw: bytes,
                   start: int,
                   icon_db: typing.Any,
                   enum_db: typing.Any):
    ident, flags, func, param, file, menu, label, icon = struct.unpack_from(">IIBBBB16sB", raw, start)
    return {
        "id": hex(ident),
        "label": label.decode("ascii").rstrip('\x00').rstrip(),
        "icon": decode_enum(icon, icon_db["items"], "id", "index", False),
        "flags": decode_flags(flags, enum_db["flags"]),
        "function": decode_enum(func, enum_db["functions"], "id", "value", True),
        "parameter": decode_enum(param, enum_db["parameters"], "id", "value", True),
        "next_file": file,
        "next_menu": "parent" if menu == 0 else menu,
    }


def decode_flags(summed: int,
                 items: typing.List[typing.Any]):
    result = []
    for bit in range(0, 24):
        mask = 1 << bit
        if (mask & summed) == 0:
            continue
        for item in items:
            if int(item['value'], 16) == mask:
                result.append(item['id'])
                if "arg" in item:
                    result.append((summed >> (4 * item['arg'])) & 0xFF)
    return result


def decode_enum(number: int,
                items: typing.List[typing.Any],
                key_id: str,
                value_id: str,
                value_is_str: bool):
    for item in items:
        if value_is_str:
            value = int(item[value_id], 16)
        else:
            value = item[value_id]
        if value == number:
            return item[key_id]
    else:
        return number


if __name__ == '__main__':
    main()
