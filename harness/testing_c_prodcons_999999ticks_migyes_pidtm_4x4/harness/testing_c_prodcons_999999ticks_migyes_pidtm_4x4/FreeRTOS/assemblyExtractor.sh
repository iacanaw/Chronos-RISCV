#!/bin/sh

X=$1

riscv-none-embed-objdump -d "$X" > "$X".S
