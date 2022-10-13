uniform sampler2D palette;
uniform highp float opacity;

varying highp float colorIndex;

lowp vec4 colorAt( float value )
{
    return texture2D( palette, vec2( value, 0.0 ) );
}

void main()
{
    gl_FragColor = colorAt( colorIndex ) * opacity;
}
