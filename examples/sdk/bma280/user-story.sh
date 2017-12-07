#!/usr/bin/env bash

set -e

cd ${JUMPER_EXAMPLES}/examples/sdk/bma280/test/
if [ -f jemu ]; then
    rm jemu
fi

python test_runner.py -v
