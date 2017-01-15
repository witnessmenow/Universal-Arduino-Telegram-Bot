#!/bin/sh -eux

platformio ci $PWD/examples/$BOARDTYPE/$EXAMPLE_FOLDER$EXAMPLE_FILE/$EXAMPLE_FILE.ino -l '.' -b $BOARD
