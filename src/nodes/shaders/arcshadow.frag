varying lowp vec2 coord;

uniform lowp vec4 color;
uniform lowp float spreadRadius;
uniform lowp float blurRadius;
uniform lowp float opacity;

/*
    arc.xy: cos/sin of the angle of the midpoint
    arc.zw: cos/sin of the angle between midpoint/endpoint
 */
uniform lowp vec4 arc;

mat2 rotation( vec2 v ) { return mat2( v.x, -v.y, v.y, v.x ); }

void main()
{
    float radius = 0.5 - blurRadius - spreadRadius;

    float dist = abs( length( coord ) - radius ) - spreadRadius;

    if ( ( arc.z < 1.0 ) && ( dist < 1.0 ) ) 
    {
        vec2 v = coord * rotation( arc.xy ); // x-axial symmetric
        v.y = abs( v.y );

        v *= rotation( arc.wz ); // end point at 90°
        if ( v.x < 0.0 )
        {
            v.y = max( 0.0, abs( v.y - radius ) - spreadRadius );
            dist = max( dist, length( v ) );
        }
    }

    float a = 1.0 - smoothstep( 0.0, blurRadius, dist );
    gl_FragColor = color * a * opacity;
}
