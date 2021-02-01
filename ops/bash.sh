#!/bin/bash

SCRIPT_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

source ${SCRIPT_HOME}/.env

docker run -it --rm --name volition-node-bash \
    --network="fallguy"                     \
    -p "9090:9090"                          \
    -v ${SCRIPT_HOME}/volume-volition:/var/lib/volition \
    --entrypoint "bash" \
    ${VOLITION_IMAGE_NAME}
