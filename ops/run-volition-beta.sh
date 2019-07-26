#!/bin/bash

{
    docker network create --driver bridge volition || true
} &> /dev/null

docker run -d --rm --name volition-node \
    --network="volition" \
    -p "80:9090" \
    -p "443:9090" \
    -v ${PWD}:/var/lib/volition \
    volition/node \
    -c /var/lib/volition/volition.properties
