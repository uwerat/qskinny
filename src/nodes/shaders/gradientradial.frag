uniform sampler2D palette;
uniform lowp float opacity;

uniform highp vec2 radius;

varying highp vec2 coord;

lowp vec4 colorAt( highp float value )
{
    return texture2D( palette, vec2( value, 0.0 ) );
}

void main()
{
    gl_FragColor = colorAt( length( coord / radius ) ) * opacity;
}
