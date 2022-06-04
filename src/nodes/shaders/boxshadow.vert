uniform highp mat4 matrix;
uniform lowp vec2 aspect;

attribute highp vec4 in_vertex;
attribute mediump vec2 in_coord;

varying mediump vec2 coord;

void main()
{
    coord = in_coord;
    gl_Position = matrix * in_vertex;
}
