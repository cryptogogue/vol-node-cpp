#!/bin/bash

#docker run --rm --name facet \
#    -p "3000:3000" \
#    facet/backend

{
    docker network create --driver bridge facet || true
} &> /dev/null

docker run -d --rm --name mysql \
    --network="facet" \
    -p 3306:3306 \
    -v ${PWD}/mysql:/var/lib/mysql \
    mysql:8.0.16
