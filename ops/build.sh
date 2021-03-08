#!/bin/bash

SCRIPT_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

pushd ${SCRIPT_HOME}
	source ./make-version-header.sh
	docker build -f docker-volition/Dockerfile -t cryptogogue/volition ../
popd
