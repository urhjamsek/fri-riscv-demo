#!/usr/bin/env python3
"""bin2mem.py -- convert a raw binary into a $readmemh image for soc_bram.v.

One 64-bit word per line, 16 hex digits. Each word is the next 8 bytes of the
binary read little-endian, so the upper 8 digits are the word at the higher
address. A final partial word is padded with zeros.

    python bin2mem.py app.bin --hex app.mem
"""
import argparse


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("bin", help="input binary (objcopy -O binary)")
    ap.add_argument("--hex", required=True, help="output .mem file")
    args = ap.parse_args()

    with open(args.bin, "rb") as f:
        data = f.read()
    data += bytes(-len(data) % 8)

    with open(args.hex, "w", newline="\n") as f:
        for i in range(0, len(data), 8):
            f.write(f"{int.from_bytes(data[i:i + 8], 'little'):016x}\n")


if __name__ == "__main__":
    main()
