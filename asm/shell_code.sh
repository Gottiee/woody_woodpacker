#!/usr/bin/env bash

nasm -felf64 asm/woody.s -o asm/woody.o

objdump -D asm/woody.o | grep '[0-9a-f]:' | cut -f2 | tr -s ' ' | tr -d '\n' > opcode
rm -f asm/payload
for f in $(cat opcode | tr ' ' '\n'); do
    echo -ne "\\\\x$f" >> asm/payload
done
echo -en $(cat asm/payload) > asm/opcode
rm asm/opcode