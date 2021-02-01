#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

docker stop volition-node
docker rm volition-node

docker run -it --rm --name volition-node-bash \
    --network="fallguy"                     \
    -p "9090:9090"                          \
    -v ${PROJECT_HOME}:/var/lib/volition \
    --entrypoint "bash" \
    volition/node
