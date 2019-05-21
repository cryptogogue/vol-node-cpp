#!/bin/bash

docker run -it --rm --name volition-market-bash \
    -v ${PWD}:/var/lib/volition-market \
    --entrypoint "bash" \
    volition/market
