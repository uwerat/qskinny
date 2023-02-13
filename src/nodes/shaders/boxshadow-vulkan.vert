#version 440

layout( location = 0 ) in vec4 in_vertex;
layout( location = 1 ) in vec2 in_coord;

layout( location = 0 ) out vec2 coord;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 color;
    vec4 radius;
    vec2 aspect;
    float blurExtent;
    float opacity;
} ubuf;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    coord = in_coord;
    gl_Position = ubuf.matrix * in_vertex;
}
