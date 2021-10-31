uniform lowp float opacity;
uniform lowp float blurExtent;
uniform lowp vec4 radius;
uniform lowp vec4 color;
uniform lowp vec2 aspect;

varying lowp vec2 coord;

lowp float effectiveRadius( in lowp vec4 radii, in lowp vec2 point )
{
    if ( point.x > 0.0 )
        return ( point.y > 0.0) ? radii.x : radii.y;
    else
        return ( point.y > 0.0) ? radii.z : radii.w;
}

void main()
{
    lowp vec4 col = vec4(0.0);

    if ( opacity > 0.0 )
    {
        const lowp float minRadius = 0.05;

        lowp float e2 = 0.5 * blurExtent;
        lowp float r = 2.0 * effectiveRadius( radius, coord );

        lowp float f = minRadius / max( r, minRadius );

        r += e2 * f;

        lowp vec2 d = r + blurExtent - aspect * ( 1.0 - abs( 2.0 * coord ) );
        lowp float l = min( max(d.x, d.y), 0.0) + length( max(d, 0.0) );

        lowp float shadow = l - r;

        lowp float v = smoothstep( -e2, e2, shadow );
        col = mix( color, vec4(0.0), v ) * opacity;
    }

    gl_FragColor = col; 
}
