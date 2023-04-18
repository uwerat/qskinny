attribute vec4 vertexCoord;

uniform mat4 matrix;
uniform highp float aspectRatio;
uniform vec2 centerCoord;

varying vec2 coord;

void main()
{
    coord = vertexCoord.xy - centerCoord;
	coord.y *= aspectRatio;

    gl_Position = matrix * vertexCoord;
}
