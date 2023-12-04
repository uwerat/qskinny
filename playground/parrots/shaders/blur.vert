uniform highp mat4 qt_Matrix;

attribute highp vec4 qt_Vertex;
attribute highp vec2 qt_MultiTexCoord0;

varying highp vec2 qt_TexCoord0;

void main()
{
    qt_TexCoord0 = qt_MultiTexCoord0;
    gl_Position = qt_Matrix * qt_Vertex;
}