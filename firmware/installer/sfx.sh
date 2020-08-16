#!/bin/sh
tail -c +155 "$0" | tar -C "/home/root/lms2012/prjs" -xzf -
rm -f "$0" "$(basename "$0" .elf).rbf"
exec "/home/root/lms2012/prjs/NXT3/nxt3.elf"
