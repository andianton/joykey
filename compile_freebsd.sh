#!/bin/sh
cc -std=c99 -Wall -Wextra -O2 \
    -I/usr/local/include \
    -L/usr/local/lib \
    -o joykey \
    xmaptest.c \
    mapper.c \
    config.c \
    joydecode.c \
    output_xtest.c \
    -lX11 -lXtst
