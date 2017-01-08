#!/bin/sh -eux

pip install --user platformio

for EXAMPLE in $PWD/examples/ESP8266/*/*.ino;
do
	platformio ci $EXAMPLE -l '.' -b $BOARD
done
