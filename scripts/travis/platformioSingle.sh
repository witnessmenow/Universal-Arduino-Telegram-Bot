#!/bin/sh -eux

platformio ci $PWD/examples/$BOARDTYPE/$EXAMPLE/$EXAMPLE.ino -l '.' -b $BOARD
