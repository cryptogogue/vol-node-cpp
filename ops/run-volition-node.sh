#!/bin/bash

#docker run --rm --name facet \
#    -p "3000:3000" \
#    facet/backend

{
    docker network create --driver bridge volition || true
} &> /dev/null


docker run --rm --name volition \
    --network="volition" \
    -p "9091:9091" \
    -v ${PWD}:/var/lib/volition \
    volition/node \
    -p 9091 -s true -k /var/lib/volition/keys/pkey0.priv.json -g /var/lib/volition/genesis.signed
