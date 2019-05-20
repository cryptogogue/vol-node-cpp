#!/bin/bash

docker run -it --rm --name volition-node-valgrind \
    -v ${PWD}:/var/lib/volition \
    --entrypoint "valgrind" \
    volition/node \
    --leak-check=yes --log-file="/var/lib/volition/valgrind.out" \
    volition -p 9090 -s true -k /var/lib/volition/keys/pkey0.priv.json -g /var/lib/volition/genesis.signed
