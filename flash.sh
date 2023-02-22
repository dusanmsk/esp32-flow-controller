#!/bin/bash
#mos flash --port=/dev/ttyUSB0 --esp-baud-rate 9600
mos flash --port=/dev/ttyUSB0 && mos console --port=/dev/ttyUSB0
