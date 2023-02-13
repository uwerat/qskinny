#version 440

layout( location = 0 ) in vec2 coord;
layout( location = 0 ) out vec4 fragColor;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 color;
    vec4 radius;
    vec2 aspect;
    float blurExtent;
    float opacity;
} ubuf;

float effectiveRadius( in vec4 radii, in vec2 point )
{
    if ( point.x > 0.0 )
        return ( point.y > 0.0) ? radii.x : radii.y;
    else
        return ( point.y > 0.0) ? radii.z : radii.w;
}

void main()
{
    vec4 col = vec4(0.0);

    if ( ubuf.opacity > 0.0 )
    {
        const float minRadius = 0.05;

        float e2 = 0.5 * ubuf.blurExtent;
        float r = 2.0 * effectiveRadius( ubuf.radius, coord );

        float f = minRadius / max( r, minRadius );

        r += e2 * f;

        vec2 d = r + ubuf.blurExtent - ubuf.aspect * ( 1.0 - abs( 2.0 * coord ) );
        float l = min( max(d.x, d.y), 0.0) + length( max(d, 0.0) );

        float shadow = l - r;

        float v = smoothstep( -e2, e2, shadow );
        col = mix( ubuf.color, vec4(0.0), v ) * ubuf.opacity;
    }

    fragColor = col; 
}
