#version 440

layout( location = 0 ) in vec4 vertexCoord;
layout( location = 0 ) out float colorIndex;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 vector;
    float opacity;
} ubuf;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    vec2 pos = vertexCoord.xy - ubuf.vector.xy;
    vec2 span = ubuf.vector.zw;

    colorIndex = dot( pos, span ) / dot( span, span );
    gl_Position = ubuf.matrix * vertexCoord;
}
