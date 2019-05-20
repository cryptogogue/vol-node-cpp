#!/bin/bash

docker run --rm --name volition-node \
    -p "9090:9090" \
    -v ${PWD}:/var/lib/volition \
    volition/node \
    -p 9090 -s true -k /var/lib/volition/keys/pkey0.priv.json -g /var/lib/volition/genesis.signed
