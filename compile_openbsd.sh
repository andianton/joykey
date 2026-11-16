#!/bin/sh
cc -std=c99 -Wall -Wextra -O2 \
    -I/usr/X11R6/include \
    -L/usr/X11R6/lib \
    -o joykey \
    xmaptest.c \
    mapper.c \
    config.c \
    joydecode.c \
    output_xtest.c \
    -lX11 -lXtst
