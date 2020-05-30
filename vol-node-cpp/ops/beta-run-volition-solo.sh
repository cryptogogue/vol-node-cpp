#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

docker stop volition-node-beta
docker rm volition-node-beta

docker run -d --name volition-node-beta     \
    --restart unless-stopped                \
    --network="fallguy"                     \
    -p "9092:9092"                          \
    -v ${PROJECT_HOME}:/var/lib/volition    \
    volition/node-beta                      \
    -p 9092 -i 1 -s true -k /var/lib/volition/.keys/root.priv.json -g /var/lib/volition/genesis --simple-recorder-folder /var/lib/volition/persist-chain-beta
