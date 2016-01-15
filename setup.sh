#!/usr/bin/env bash

echo "ml-examples-oF: Checking for changes."
git pull origin master # check for updates to the code

echo "ml-examples-oF: Initializing all submodules."
git submodule init # registers submodules with git

git submodule update

echo "ml-examples-oF: Pulling all submodules."
git submodule foreach git pull origin master # iterate through submodules and pull updates

cd addons/ofxCcv
./setup.sh
cd - &>/dev/null