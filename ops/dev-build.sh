#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

source ${PROJECT_HOME}/ops/make-version-header.sh
docker build -t volition/node-dev .
