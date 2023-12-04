#version 440

layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(binding = 1) uniform sampler2D source;

layout(std140, binding = 0) uniform buf
{
    mat4 qt_Matrix;
    float qt_Opacity;

} ubuf;

void main()
{
    vec2 delta = vec2(0.01, 0.01);

    fragColor =(0.0538 * texture(source, qt_TexCoord0 - 3.182 * delta)
                + 0.3229 * texture(source, qt_TexCoord0 - 1.364 * delta)
                + 0.2466 * texture(source, qt_TexCoord0)
                + 0.3229 * texture(source, qt_TexCoord0 + 1.364 * delta)
                + 0.0538 * texture(source, qt_TexCoord0 + 3.182 * delta)) * ubuf.qt_Opacity;
}
