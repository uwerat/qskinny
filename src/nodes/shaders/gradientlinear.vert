attribute vec4 vertexCoord;

uniform mat4 matrix;
uniform vec4 rect;

varying float colorIndex;

void main()
{
    highp vec2 l = vertexCoord.xy - rect.xy;
    highp vec2 size = rect.zw;

    colorIndex = dot( l, size ) / dot( size, size );
    gl_Position = matrix * vertexCoord;
}
