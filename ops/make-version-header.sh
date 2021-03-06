#!/usr/bin/env bash 

SCRIPT_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

PROJECT_HOME="${SCRIPT_HOME}/../"
FOLDER="${PROJECT_HOME}/src/.config/volition"
FILENAME="${FOLDER}/version.h"

pushd ${PROJECT_HOME}

	mkdir -p $FOLDER

	GIT_TAG=$(git describe --long --dirty --always --tags)
	GIT_COMMIT=$(git rev-parse HEAD)
	BUILD_DATE=$(date +"%D %T %Z")

	printf "#ifndef VOLITION_VERSION_H\n" > $FILENAME
	printf "#define VOLITION_VERSION_H\n" >> $FILENAME
	printf "\n" >> $FILENAME

	printf "#define VOLITION_BUILD_DATE_STR \"%s\"\n" "$BUILD_DATE" >> $FILENAME
	printf "#define VOLITION_GIT_COMMIT_STR \"%s\"\n" "$GIT_COMMIT" >> $FILENAME
	printf "#define VOLITION_GIT_TAG_STR \"%s\"\n" "$GIT_TAG" >> $FILENAME

	printf "\n" >> $FILENAME
	printf "#endif\n" >> $FILENAME

popd