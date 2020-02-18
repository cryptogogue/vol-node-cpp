#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

docker run -d --name volition-node          \
    --restart unless-stopped                \
    --network="fallguy"                     \
    -p "9090:9090"                          \
    -v ${PROJECT_HOME}:/var/lib/volition    \
    volition/node                           \
    -p 9090 -i 60 -s true -k /var/lib/volition/.keys/root.priv.json -g /var/lib/volition/genesis --simple-recorder-folder /var/lib/volition/persist-chain
