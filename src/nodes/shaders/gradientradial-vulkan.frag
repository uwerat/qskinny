#version 440

layout( location = 0 ) in vec2 coord;
layout( location = 0 ) out vec4 fragColor;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec2 centerCoord;
    vec2 radius;
    float opacity;
} ubuf;

layout( binding = 1 ) uniform sampler2D colorRamp;

vec4 colorAt( float value )
{
    return texture( colorRamp, vec2( value, 0.0 ) );
}

void main()
{
    fragColor = colorAt( length( coord / ubuf.radius ) ) * ubuf.opacity;
}
