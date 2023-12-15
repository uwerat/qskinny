#version 440

layout( location = 0 ) in vec2 coord;
layout( location = 0 ) out vec4 fragColor;

layout( binding = 1 ) uniform sampler2D source;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    float opacity;
} ubuf;

void main()
{
    vec4 c = texture( source, coord );
    fragColor = c.yzxw * ubuf.opacity;;
}
