uniform sampler2D source;
uniform lowp float opacity;

varying highp vec2 coord;

void main()
{
    vec2 delta = vec2( 0.01, 0.01 );

    gl_FragColor =(
        0.0538 * texture2D( source, coord - 3.182 * delta )
        + 0.3229 * texture2D( source, coord - 1.364 * delta )
        + 0.2466 * texture2D( source, coord )
        + 0.3229 * texture2D( source, coord + 1.364 * delta )
        + 0.0538 * texture2D( source, coord + 3.182 * delta)
    ) * opacity;
}
