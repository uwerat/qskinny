#!/bin/bash

SVG2QVG=$1
SVG=$2
QVG=$3

LD_LIBRARY_PATH=$4:$LD_LIBRARY_PATH $SVG2QVG $SVG $QVG