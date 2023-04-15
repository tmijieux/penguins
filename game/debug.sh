#!/bin/bash

cd $(dirname $0)
export LD_LIBRARY_PATH=./lib:./map:./clients
exec gdb --args ./penguins_server "$@"
