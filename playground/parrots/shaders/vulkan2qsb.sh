#! /bin/sh 

function qsbcompile {
    qsbfile=`echo $1 | sed 's/-vulkan//'`
    qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o ${qsbfile}.qsb $1
}

qsbcompile blur-vulkan.vert
qsbcompile blur-vulkan.frag
qsbcompile rgbswap-vulkan.frag
