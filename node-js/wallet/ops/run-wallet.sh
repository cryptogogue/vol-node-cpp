#!/bin/bash

docker run -d --rm --name volition-wallet \
    -p "9999:80" \
    -v ${PWD}:/var/lib/volition-wallet \
    volition/wallet
