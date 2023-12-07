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
    vec2 delta = vec2( 0.01, 0.01 );

    fragColor =(
        0.0538 * texture( source, coord - 3.182 * delta )
        + 0.3229 * texture( source, coord - 1.364 * delta )
        + 0.2466 * texture( source, coord )
        + 0.3229 * texture( source, coord + 1.364 * delta )
        + 0.0538 * texture( source, coord + 3.182 * delta )
    ) * ubuf.opacity;
}
