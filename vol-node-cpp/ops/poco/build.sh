#!/bin/bash

SCRIPT_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
PROJECT_HOME=${SCRIPT_HOME}/../../

pushd ${PROJECT_HOME}

    mkdir -p build/build-poco
    cd build/build-poco
    cmake ${PROJECT_HOME}3rd-party/poco \
        -DPOCO_STATIC=1 \
        -DDISABLE_CPP11=ON  \
            -DDISABLE_CPP14=ON  \
            -DENABLE_MONGODB=OFF \
            -DENABLE_REDIS=OFF  \
            -DENABLE_PDF=OFF    \
            -DENABLE_DATA=OFF   \
            -DENABLE_DATA_SQLITE=OFF \
            -DENABLE_DATA_MYSQL=OFF \
            -DENABLE_DATA_POSTGRESQL=OFF \
            -DENABLE_DATA_ODBC=OFF \
            -DENABLE_PAGECOMPILER=OFF \
            -DENABLE_PAGECOMPILER_FILE2PAGE=OFF
    make -s -j4
    make package

popd
