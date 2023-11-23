attribute highp vec4 in_vertex;
uniform highp mat4 matrix;

uniform lowp vec2 size;

float normalized( in float pos, in float scale, in float size )
{
    return ( ( pos / size - 0.5 ) / 0.5 ) * scale;
}

float denormalized( in float pos, in float scale, in float size )
{
    return ( ( pos / scale ) * 0.5 + 0.5 ) * size;
}

float round( in float v )
{
    return floor( v + 0.5 );
}

void main()
{
    gl_Position = matrix * in_vertex;

    if ( size.x > 0.0 )
    {
        gl_Position.x = denormalized( gl_Position.x, gl_Position.w, size.x );
        gl_Position.x = round( gl_Position.x ) + 0.5;
        gl_Position.x = normalized( gl_Position.x, gl_Position.w, size.x );
    }

    if ( size.y > 0.0 )
    {
        gl_Position.y = denormalized( gl_Position.y, gl_Position.w, size.y );
        gl_Position.y = round( gl_Position.y ) + 0.5;
        gl_Position.y = normalized( gl_Position.y, gl_Position.w, size.y );
    }
}
