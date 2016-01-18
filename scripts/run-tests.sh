#!/usr/bin/env bash

set -e

TEST_USER="test_server_user_$(date +%s)"

function cleanup {
    echo "Killing all clanchat server processes"
    # Kill background test server without error
    pkill -f "clanchat ${TEST_USER}"

    if pgrep -f "clanchat ${TEST_USER}" > /dev/null; then
        # Only sleep if kill didn't work
        sleep 0.2
    fi
    
    if pgrep -f "clanchat ${TEST_USER}" > /dev/null; then
        echo "Processes didn't die; resorting to SIGKILL"
        # If it's still there, really kill it without error
        pkill -f "clanchat testuser" -9
    fi

    # Reset current directory
    popd
}
trap cleanup EXIT

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

# Set current directory to root of repository
pushd `dirname $0`/..

# Build updated binaries
make clean && make

# Start test server
./src/clanchat ${TEST_USER} &
PID=$!
echo "Started clanchat server in process ${PID}"
 
# Wait for port to become available
wait_for_port 37777

# Run tests
make check

# Test server gets killed in exit trap
exit

