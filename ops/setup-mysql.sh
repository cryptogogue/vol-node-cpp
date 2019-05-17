#!/bin/bash

docker run --name mysql \
    --network="none" \
    -v ${PWD}/mysql:/var/lib/mysql \
    -v ${PWD}/sql-initdb:/docker-entrypoint-initdb.d \
    -e MYSQL_ROOT_PASSWORD=password \
    mysql:8.0.16
