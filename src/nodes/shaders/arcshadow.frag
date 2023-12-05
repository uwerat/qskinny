uniform lowp float qt_Opacity;

uniform lowp vec4 color;
uniform lowp vec4 rect;
uniform lowp float radius;
uniform lowp float thickness;
uniform lowp float startAngle;
uniform lowp float spanAngle;
uniform lowp float extend;

const float M_PI = 3.141592653589793;

float sdRing( in vec2 p, in vec2 n, in float r, in float th )
{
    p.x = abs(p.x);
    
    p = mat2(n.x,n.y,-n.y,n.x)*p;

    return max( abs(length(p)-r)-th*0.5,
                length(vec2(p.x,max(0.0,abs(r-p.y)-th*0.5)))*sign(p.x) );
}

void main()
{
    // uniforms
    float shadowSize = extend;           // px >= 0.0
    vec2 iResolution = rect.zw;       

    // normalized pixel coordinates
    vec2 p = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.xy; // xy for ellipse
    float shadowSizeUv = shadowSize / min(iResolution.x, iResolution.y);
             
    float t = radians(abs(spanAngle) / 2.0);
    vec2 cs = vec2(cos(t),sin(t));
  
    // rotation
    float ra = radians(startAngle + spanAngle / 2.0);
    {
        float sin_ra = sin(ra);
        float cos_ra = cos(ra);
        p = mat2(cos_ra, -sin_ra, sin_ra, cos_ra) * p;
    }
  
    // distance
    float d = sdRing(p, cs, radius, thickness);
    float e = 1.0 / shadowSizeUv;
        
    // coloring
    float v = 1.0 - abs(d) * e;
    float a = d >= 0.0 && abs(d) < e ? v : 0.0; // alpha
    gl_FragColor = vec4(color.rgb, a) * qt_Opacity;
}