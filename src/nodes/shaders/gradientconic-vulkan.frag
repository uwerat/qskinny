#version 440

layout( location = 0 ) in vec2 coord;
layout( location = 0 ) out vec4 fragColor;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec2 centerCoord;
    float start;
    float span;
    float opacity;
} ubuf;

layout( binding = 1 ) uniform sampler2D colorRamp;

vec4 colorAt( highp float value )
{
    return texture( colorRamp, vec2( value, 0.0 ) );
}

void main()
{
    /*
        angles as ratio of a rotation:
            start: [ 0.0, 1.0 [
            span:  ] -1.0, 1.0 [
     */

    float v = sign( ubuf.span ) * ( atan( -coord.y, coord.x ) / 6.2831853 - ubuf.start );
    fragColor = colorAt( ( v - floor( v ) ) / abs( ubuf.span ) ) * ubuf.opacity;
}
