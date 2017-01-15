#!/bin/sh -eux

platformio ci $PWD/examples/$BOARDTYPE$EXAMPLE_FOLDER$EXAMPLE_NAME/$EXAMPLE_NAME.ino -l '.' -b $BOARD
