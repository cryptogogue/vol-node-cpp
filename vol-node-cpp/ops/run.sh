#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

docker stop volition-node
docker rm volition-node

# docker run -d --name volition-node          		\
#     --restart unless-stopped                		\
#     -p "9091:9091"                          		\
#     -v ${PROJECT_HOME}/volume:/var/lib/volition		\
#     volition/node                           		\
#     -c volition.ini

docker run --name volition-node          			\
    -p "9091:9091"                          		\
    -v ${PROJECT_HOME}/volume:/var/lib/volition		\
    volition/node
