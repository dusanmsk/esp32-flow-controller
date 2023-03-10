#!/bin/bash

#mos build --local --platform esp32 --build-var BOARD=ESP32-EVB $@  && \
mos build --local --platform esp32 $@  && \
mos flash && \
mos console

