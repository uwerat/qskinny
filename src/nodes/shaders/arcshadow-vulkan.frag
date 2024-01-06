#version 440

layout( location = 0 ) in vec2 coord; 
layout( location = 0 ) out vec4 fragColor;

layout( std140, binding = 0 ) uniform buf
{
    mat4 matrix;
    vec4 color;

    /*
        arc.xy: cos/sin of the angle of the midpoint
        arc.zw: cos/sin of the angle between midpoint/endpoint
    */
    vec4 arc;

    float spreadRadius;
    float blurRadius;

    float opacity;
} ubuf;

mat2 rotation( vec2 v ) { return mat2( v.x, -v.y, v.y, v.x ); }

void main()
{        
    float radius = 0.5 - ubuf.blurRadius - ubuf.spreadRadius;

    float dist = abs( length( coord ) - radius ) - ubuf.spreadRadius;

    if ( ( ubuf.arc.z ) < 1.0 && ( dist < 1.0 ) )
    {
        vec2 v = coord * rotation( ubuf.arc.xy ); // x-axial symmetric
        v.y = abs( v.y );

        v *= rotation ( ubuf.arc.wz ); // end point to 90°
        if ( v.x < 0.0 )
        {
            v.y = max( 0.0, abs( v.y - radius ) - ubuf.spreadRadius );
            dist = max( dist, length( v ) );
        }
    }

    float a = 1.0 - smoothstep( 0.0, ubuf.blurRadius, dist );
    fragColor = ubuf.color * a * ubuf.opacity;
}
