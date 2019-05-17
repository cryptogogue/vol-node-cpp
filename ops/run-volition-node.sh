#!/bin/bash

#docker run --rm --name facet \
#    -p "3000:3000" \
#    facet/backend

{
    docker network create --driver bridge facet || true
} &> /dev/null


docker run --rm --name facet \
    --network="facet" \
    -p "3000:3000" \
    facet/backend
