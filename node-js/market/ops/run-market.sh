#!/bin/bash

{
    docker network create --driver bridge volition || true
} &> /dev/null

docker run -d --rm --name volition-market \
    --network="volition" \
    -p "7777:7777" \
    -v ${PWD}:/var/lib/volition-market \
    volition/market
