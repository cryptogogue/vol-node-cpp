#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

docker stop volition-node
docker rm volition-node

docker run --rm --name volition-node \
    --network="volition" \
    -p "9090:9090" \
    -v ${PROJECT_HOME}:/var/lib/volition \
    volition/node \
    -p 9090 -s true -k /var/lib/volition/keys/key9090.priv.json -g /var/lib/volition/genesis
