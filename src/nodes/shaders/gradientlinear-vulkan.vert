#version 440

layout( location = 0 ) in vec4 vertexCoord;
layout( location = 0 ) out float colorIndex;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 rect;
    float opacity;
} ubuf;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    vec2 l = vertexCoord.xy - ubuf.rect.xy;
    vec2 size = ubuf.rect.zw;

    colorIndex = dot( l, size ) / dot( size, size );
    gl_Position = ubuf.matrix * vertexCoord;
}
