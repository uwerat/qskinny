attribute highp vec4 vertex;
uniform highp mat4 qt_Matrix;

void main()
{
    gl_Position = qt_Matrix * vertex;
}