#!/bin/bash
source ./env

echo "Getting configuration"
mos get conf9.json > conf9.json

echo "Flashing ..."
mos config-set debug.level=0
#curl -v -F file=@build/fw.zip http://${ESP_IP_ADDRESS}/update
mos ota --port ws://$ESP_IP_ADDRESS/rpc build/fw.zip

echo "Restoring configuration"
mos put conf9.json

echo "Sleeping, then commiting ..."
sleep 5
mos call Ota.Commit
echo "Image commited"



