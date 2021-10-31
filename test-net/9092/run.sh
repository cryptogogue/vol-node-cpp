#!/bin/bash

SCRIPT_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

SERVER_BIN="${SCRIPT_HOME}/server"

if [[ ! -f "$SERVER_BIN" ]]; then
    SERVER_BIN="${SCRIPT_HOME}/../../bin/server"
fi

${SERVER_BIN} -c volition.ini
