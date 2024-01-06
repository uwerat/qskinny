#! /bin/sh 

function qsbcompile {
    qsbfile=`echo $1 | sed 's/-vulkan//'`
    qsb --glsl 100es,120,150 --hlsl 50 --msl 12 -b -o ${qsbfile}.qsb $1
    # qsb --qt6 -b -o ${qsbfile}.qsb $1
} 

qsbcompile arcshadow-vulkan.vert
qsbcompile arcshadow-vulkan.frag
