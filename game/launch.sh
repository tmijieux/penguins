#!/bin/bash

cd $(dirname $0)
export PATH=./lib:./map:./clients:$PATH # for windows dll
export LD_LIBRARY_PATH=./lib:./map:./clients
echo $PATH
exec ./penguins_server "$@"
