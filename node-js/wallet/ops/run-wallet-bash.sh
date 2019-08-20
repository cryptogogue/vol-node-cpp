#!/bin/bash

docker run -it --rm --name volition-wallet-bash \
    -v ${PWD}:/var/lib/volition-wallet \
    --entrypoint "bash" \
    volition/wallet
