attribute vec4 vertexCoord;

uniform mat4 matrix;
uniform vec4 vector;

varying float colorIndex;

void main()
{
    highp vec2 pos = vertexCoord.xy - vector.xy;
    highp vec2 span = vector.zw;

    colorIndex = dot( pos, span ) / dot( span, span );
    gl_Position = matrix * vertexCoord;
}
