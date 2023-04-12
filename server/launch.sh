#!/bin/bash

cd $(dirname $0)
export LD_LIBRARY_PATH=./lib:./map:./clients
exec ./penguins_server "$@"
