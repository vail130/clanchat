#!/usr/bin/env bash

set -e

CURRENT_DIR=`dirname $0`
pushd ${CURRENT_DIR}/..

function reset_current_directory {
    popd
    if [ ! -z $PID ]; then
        kill $PID
    fi
}
trap reset_current_directory EXIT

function wait_for_port() {
    COUNT=0
    until nc -v -w 1 -z "127.0.0.1" $1
    do
        if [ "${COUNT}" -gt "3" ]
        then
            echo "Timed out!"
            exit 1
        fi
        COUNT=$((${COUNT} + 1))
        sleep 1
    done
}

# Build updated binaries
make clean && make

# Start test server
./src/clanchat testuser &
PID=$! # kill in exit trap above
echo "Started clanchat server in process ${PID}"
 
# Wait for port to become available
wait_for_port 37777

# Run tests
make check

# Kill background test server without error
kill $PID 2> /dev/null
PID=

