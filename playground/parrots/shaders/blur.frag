varying highp vec2 qt_TexCoord0;

uniform sampler2D source;
uniform lowp float qt_Opacity;

void main()
{
    vec2 delta = vec2(0.01, 0.01);

    gl_FragColor =(0.0538 * texture2D(source, qt_TexCoord0 - 3.182 * delta)
                 + 0.3229 * texture2D(source, qt_TexCoord0 - 1.364 * delta)
                 + 0.2466 * texture2D(source, qt_TexCoord0)
                 + 0.3229 * texture2D(source, qt_TexCoord0 + 1.364 * delta)
                 + 0.0538 * texture2D(source, qt_TexCoord0 + 3.182 * delta)) * qt_Opacity;
}
