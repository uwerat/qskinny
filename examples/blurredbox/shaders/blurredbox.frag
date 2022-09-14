// opacity of the rectangle
uniform lowp float rectOpacity;
// x = top left, y = top right, z = bottom right, w = bottom left
uniform lowp vec4 rectCornerRadii;
// the rectangle's aspect ratio
uniform lowp vec2 rectAspect;

// must be greater than 0.0!
uniform lowp float blurDirections;
// must be greater than 0.0!
uniform lowp float blurQuality;
// the radius for blurring
uniform lowp vec2 blurRadius;

// normalized position of the fragment
varying lowp vec2 coord;

// the texture with coordinates from top left (0.0,0.0) to bottom right (1.0,1.0)
uniform sampler2D txr;

uniform lowp float edgeSoftness;

lowp float effectiveRadius( in lowp vec4 radii, in lowp vec2 point )
{
    if ( point.y < 0.5 )
        return ( point.x < 0.5) ? radii.x : radii.y;
    else
        return ( point.x < 0.5) ? radii.z : radii.w;
}

lowp float roundedBoxSDF(lowp vec2 centerPosition, lowp vec2 size, lowp float radius)
{
    return length( max( abs(centerPosition) - size + radius, 0.0) ) - radius;
}

void main()
{
    if(rectOpacity == 0.0)
    {
        return;
    }

    lowp vec2 rectAspectHalf = rectAspect / 2.0;

    // Pixel color
    lowp vec4 fragColorOrig = texture2D(txr, coord);
    lowp vec4 fragColor = fragColorOrig;

    // Blur calculations
    const lowp float Pi = 6.28318530718; // constant for Pi*2
    for(lowp float d = 0.0; d < Pi; d += Pi / blurDirections)
    {
        lowp vec2 offset = vec2(cos(d), sin(d));
        for(lowp float i = 1.0 / blurQuality; i <= 1.0; i += 1.0 / blurQuality)
        {
            fragColor += texture2D( txr, coord + offset * blurRadius * i);
        }
    }

    // Current corner radius
    lowp float cornerRadius = effectiveRadius(rectCornerRadii, coord);

    // Calculate distance to edge.    
    lowp float distance = roundedBoxSDF(coord * rectAspect - rectAspectHalf, rectAspectHalf, cornerRadius);

    // Smooth the result (free antialiasing).
    lowp float smoothedAlpha = 1.0 - smoothstep(0.0, 2.0 * edgeSoftness, distance);

    // Output to screen
    fragColor /= blurQuality * blurDirections - 15.0;

    // Mix the original pixel coolor with the blurred
    gl_FragColor = mix(fragColorOrig, vec4(fragColor.xyz, 1.0), smoothedAlpha) * rectOpacity;
}

// further reading
// - https://iquilezles.org/articles/distfunctions
// - https://www.shadertoy.com/view/Xltfzj
