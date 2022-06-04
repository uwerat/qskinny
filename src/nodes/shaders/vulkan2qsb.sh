#! /bin/sh 

qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o  boxshadow.vert.qsb boxshadow-vulkan.vert
qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o  boxshadow.frag.qsb boxshadow-vulkan.frag
