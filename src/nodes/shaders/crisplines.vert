attribute highp vec4 in_vertex;
uniform highp mat4 matrix;

uniform lowp vec2 origin;

float round( in float v )
{
    return floor( v + 0.5 );
}

void main()
{
    vec4 pos = matrix * in_vertex;

    if ( origin.x > 0.0 )
    {
        pos.x = ( pos.x + 1.0 ) * origin.x;
        pos.x = round( pos.x ) + 0.5;
        pos.x = pos.x / origin.x - 1.0;
    }

    if ( origin.y > 0.0 )
    {
        pos.y = ( pos.y + 1.0 ) * origin.y;
        pos.y = round( pos.y ) + 0.5;
        pos.y = pos.y / origin.y - 1.0;
    }

    gl_Position = pos;
}
