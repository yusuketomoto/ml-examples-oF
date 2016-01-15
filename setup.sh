#!/usr/bin/env bash

echo "ml-examples-oF: Checking for changes."
git pull origin master # check for updates to the code

echo "ml-examples-oF: Initializing submodules."
git submodule init # registers submodules with git

echo "ml-examples-oF: Updating submodules."
git submodule update # pulls content of submodule repos
