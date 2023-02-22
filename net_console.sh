#!/bin/bash
source ./env

mos config-set debug.level=3 debug.udp_log_addr=${THIS_PC_ADDRESS}:7777
socat -u udp-recv:7777 -
