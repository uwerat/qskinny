#version 440

layout( location = 0 ) in vec2 coord; // [-1.0,+1.0]x[-1.0,+1.0]
layout( location = 0 ) out vec4 fragColor;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 color;       // shadow's color
    vec2 offset;      // shadow's offset (x,y) : [-1.0, +1.0]x[-1.0,+1.0]
    float radius;     // arc's radius [0.0, 1.0]
    float thickness;  // arc's thickness [0.0, 1.0]
    float startAngle; // arc's start angle [rad]
    float spanAngle;  // arc's span angle [rad]
    float extend;     // shadow length [0.0, 1.0]
    float opacity;    // overall opacity [0.0, 1.0]
} ubuf;

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
    vec2 p = coord + ubuf.offset; 
    float ra = -ubuf.startAngle - ubuf.spanAngle / 2.0;
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

    float t = abs(ubuf.spanAngle) / 2.0; // half span angle
    vec2 cs = vec2(cos(t),sin(t));
    float d = sdRing(p, cs, ubuf.radius / 2.0, ubuf.thickness);

    float a = 1.0 - smoothstep(0.0, ubuf.extend, d);
    fragColor = ubuf.color * a * ubuf.opacity;
}