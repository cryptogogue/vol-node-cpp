#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

docker stop volition-node-dev
docker rm volition-node-dev

docker run -it --rm --name volition-node-bash-dev \
    --network="fallguy"                     \
    -p "9091:9091"                          \
    -v ${PROJECT_HOME}:/var/lib/volition \
    --entrypoint "bash" \
    volition/node-dev
