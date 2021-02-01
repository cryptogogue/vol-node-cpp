#!/bin/bash

SCRIPT_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

pushd ${SCRIPT_HOME}
    docker-compose down -t 60
popd
