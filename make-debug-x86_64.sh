#!/bin/bash

workspaceFolder=$(cd "$(dirname "$0")" && pwd)
echo "$workspaceFolder"

cd ${workspaceFolder} && make clean && make BUILD=DEBUG

cd ${workspaceFolder}
