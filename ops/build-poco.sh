#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/../ && pwd)

mkdir -p build/build-poco
cd ${PROJECT_HOME}/build/build-poco
cmake ../../3rd-party/poco \
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
