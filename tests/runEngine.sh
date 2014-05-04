#!/bin/sh

ARG1="$1"
echo "Running Cura Engine - with default.cfg"
../build/CuraEngine -v -p -c default.cfg -o $1.gcode $1.stl
