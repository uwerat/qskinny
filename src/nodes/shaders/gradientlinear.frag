uniform sampler2D colorRamp;
uniform highp float opacity;

varying highp float colorIndex;

lowp vec4 colorAt( float value )
{
    return texture2D( colorRamp, vec2( value, 0.0 ) );
}

void main()
{
    gl_FragColor = colorAt( colorIndex ) * opacity;
}
