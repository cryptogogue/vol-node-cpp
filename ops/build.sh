#!/bin/bash

SCRIPT_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

source ${SCRIPT_HOME}/.env

if [[ -z "${VOLITION_IMAGE_NAME}" ]]; then
	echo "VOLITION_IMAGE_NAME is undefined"
	exit 1
fi

pushd ${SCRIPT_HOME}
	source ./make-version-header.sh
	docker build -f docker-volition/Dockerfile -t ${VOLITION_IMAGE_NAME} ../
popd
