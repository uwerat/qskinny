attribute vec4 vertexCoord;

uniform mat4 matrix;
uniform vec2 centerCoord;

varying vec2 coord;

void main()
{
    coord = vertexCoord.xy - centerCoord;
    gl_Position = matrix * vertexCoord;
}
