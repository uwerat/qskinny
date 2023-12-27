uniform sampler2D source;
uniform lowp float opacity;

varying highp vec2 coord;

void main()
{
	vec4 c = texture2D( source, coord );
	gl_FragColor = c.yzxw * opacity;
}
