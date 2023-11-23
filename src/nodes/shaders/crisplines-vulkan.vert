#version 440

layout( location = 0 ) in vec4 vertexCoord;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 color;
    vec2 size;
} ubuf;

out gl_PerVertex { vec4 gl_Position; };

float normalized( in float pos, in float scale, in float size )
{
    return ( ( pos / size - 0.5 ) / 0.5 ) * scale;
}

float denormalized( in float pos, in float scale, in float size )
{
    return ( ( pos / scale ) * 0.5 + 0.5 ) * size;
}

void main()
{
    gl_Position = ubuf.matrix * vertexCoord;

    if ( ubuf.size.x > 0.0 )
    {
        gl_Position.x = denormalized( gl_Position.x, gl_Position.w, ubuf.size.x );
        gl_Position.x = round( gl_Position.x ) + 0.5;
        gl_Position.x = normalized( gl_Position.x, gl_Position.w, ubuf.size.x );
    }

    if ( ubuf.size.y > 0.0 )
    {
        gl_Position.y = denormalized( gl_Position.y, gl_Position.w, ubuf.size.y );
        gl_Position.y = round( gl_Position.y ) + 0.5;
        gl_Position.y = normalized( gl_Position.y, gl_Position.w, ubuf.size.y );
    }
}
