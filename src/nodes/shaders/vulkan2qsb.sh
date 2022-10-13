#! /bin/sh 

function qsbcompile {
    qsbfile=`echo $1 | sed 's/-vulkan//'`
    qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o ${qsbfile}.qsb $1
} 

qsbcompile boxshadow-vulkan.vert
qsbcompile boxshadow-vulkan.frag

qsbcompile gradientconic-vulkan.vert
qsbcompile gradientconic-vulkan.frag

qsbcompile gradientradial-vulkan.vert
qsbcompile gradientradial-vulkan.frag

qsbcompile gradientlinear-vulkan.vert
qsbcompile gradientlinear-vulkan.frag
