#!/bin/bash

SOURCE=${BASH_SOURCE[0]}
DIR=$( dirname "$SOURCE" )

if [ ! -d "$DIR/src/gen" ]; then
	mkdir $DIR/src/gen
fi

pushd $DIR/resources
xxd -i dbtemplate.db3 > $DIR/src/gen/dbtemplate_db3_i.hpp
popd

