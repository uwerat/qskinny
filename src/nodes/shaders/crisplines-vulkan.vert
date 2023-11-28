#version 440

layout( location = 0 ) in vec4 vertexCoord;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 color;
    vec2 origin;
} ubuf;

out gl_PerVertex { vec4 gl_Position; };

void main()
{
    vec4 pos = ubuf.matrix * vertexCoord;

    if ( ubuf.origin.x > 0.0 )
    {
        pos.x = ( pos.x + 1.0 ) * ubuf.origin.x;
        pos.x = round( pos.x ) + 0.5;
        pos.x = pos.x / ubuf.origin.x - 1.0;
    }

    if ( ubuf.origin.y > 0.0 )
    {
        pos.y = ( pos.y + 1.0 ) * ubuf.origin.y;
        pos.y = round( pos.y ) + 0.5;
        pos.y = pos.y / ubuf.origin.y - 1.0;
    }

    gl_Position = pos;
}
