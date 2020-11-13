#!/bin/bash

PROJECT_HOME=$(cd $(dirname "${0}")/ && pwd)

rm ${PROJECT_HOME}/9090/persist-chain/*.*
rm ${PROJECT_HOME}/9091/persist-chain/*.*
rm ${PROJECT_HOME}/9092/persist-chain/*.*
rm ${PROJECT_HOME}/9093/persist-chain/*.*

exit 0
