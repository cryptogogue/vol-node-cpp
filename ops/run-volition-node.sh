#!/bin/bash

#docker run --rm --name facet \
#    -p "3000:3000" \
#    facet/backend

{
    docker network create --driver bridge volition || true
} &> /dev/null


docker run --rm --name volition \
    --network="volition" \
    -p "9090:9090" \
    -v ${PWD}/secrets:./secrets \
    volition/node
