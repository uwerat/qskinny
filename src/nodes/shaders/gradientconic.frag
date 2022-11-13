uniform sampler2D colorRamp;
uniform lowp float opacity;

uniform highp float start;
uniform highp float span;

varying highp vec2 coord;

lowp vec4 colorAt( highp float value )
{
    return texture2D( colorRamp, vec2( value, 0.0 ) );
}

void main()
{
    /*
        angles as ratio of a rotation:
            start: [ 0.0, 1.0 [
            span:  ] -1.0, 1.0 [
     */

    highp float v = sign( span ) * ( atan( -coord.y, coord.x ) / 6.2831853 - start ); 
    gl_FragColor = colorAt( ( v - floor( v ) ) / abs( span ) ) * opacity;
}
