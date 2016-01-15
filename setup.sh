#!/usr/bin/env bash

echo "ml-examples-oF: Checking for changes."
git pull origin pull --recurse-submodules

echo "ml-examples-oF: Initializing submodules."
git submodule init

echo "ml-examples-oF: Updating submodules."
git submodule update --remote --merge

cd addons/ofxCcv
./setup.sh
cd - &>/dev/null