#!/bin/bash

docker run -it --rm --name volition-node-bash-dev \
    -v ${PWD}:/var/lib/volition \
    --entrypoint "bash" \
    volition/node-dev
