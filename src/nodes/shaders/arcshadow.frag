varying lowp vec2 coord;         // [-1.0,+1.0]x[-1.0,+1.0]

uniform lowp mat4 matrix;
uniform lowp vec4 color;         // shadow's color
uniform lowp vec2 offset;        // shadow's offset (x,y) : [-1.0, +1.0]x[-1.0,+1.0]
uniform lowp float radius;       // arc's radius [0.0, 1.0]
uniform lowp float thickness;    // arc's thickness [0.0, 1.0]
uniform lowp float startAngle;   // arc's start angle [rad]
uniform lowp float spanAngle;    // arc's span angle [rad]
uniform lowp float spreadRadius; // extended shadow length [0.0, 1.0]
uniform lowp float blurRadius;   // extended shadow blurring length [0.0, 1.0]
uniform lowp float opacity;      // overall opacity [0.0, 1.0]

float sdRing( in vec2 p, in vec2 n, in float r, in float th )
{
    p.x = abs(p.x);
    
    p = mat2(n.x,n.y,-n.y,n.x)*p;

    return max( abs(length(p)-r)-th*0.5,
                length(vec2(p.x,max(0.0,abs(r-p.y)-th*0.5)))*sign(p.x) );
}

void main()
{        
    // rotation
    vec2 p = coord + offset; 
    float ra = -startAngle - spanAngle / 2.0;
    {        
        float sin_ra = sin(ra);
        float cos_ra = cos(ra);
        mat2 transform = mat2
        ( 
            cos_ra, -sin_ra,
            sin_ra,  cos_ra
        );
    
        p = transform * p;
    }

    float t = abs(spanAngle) / 2.0; // half span angle
    vec2 cs = vec2(cos(t),sin(t));
    float d = sdRing(p, cs, radius / 2.0, thickness + spreadRadius);

    float a = 1.0 - smoothstep(0.0, blurRadius, d);
    gl_FragColor = color * a * opacity;
}