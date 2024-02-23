#!/bin/bash

SOURCE=${BASH_SOURCE[0]}
DIR=$( dirname "$SOURCE" )

if [ ! -d "$DIR/src/gen" ]; then
	mkdir $DIR/src/gen
fi

pushd $DIR/resources
xxd -i code_gen_template.hpp > $DIR/src/gen/code_gen_template_i.hpp
popd

