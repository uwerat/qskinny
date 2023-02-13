#version 440

layout( location = 0 ) in float colorIndex;
layout( location = 0 ) out vec4 fragColor;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 vector;
    float opacity;
} ubuf;

layout( binding = 1 ) uniform sampler2D colorRamp;

vec4 colorAt( float value )
{
    return texture( colorRamp, vec2( value, 0.0 ) );
}

void main()
{
    fragColor = colorAt( colorIndex ) * ubuf.opacity;
}
