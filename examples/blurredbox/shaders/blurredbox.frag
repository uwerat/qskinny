// opacity of the rectangle
uniform lowp float rectOpacity;
// x = top left, y = top right, z = bottom right, w = bottom left
uniform lowp vec4 rectCornerRadii;
// the rectangle on the screen (x, y, width, height)
uniform lowp vec4 rectOnScreen;
// the screen's rectangle (x, y, width, height)
uniform lowp vec4 rectOfScreen;
// must be greater than 0.0!
uniform lowp float blurDirections;
// must be greater than 0.0!
uniform lowp float blurQuality;
// the radius for blurring
uniform lowp float blurSize;

// normalized position of the fragment
varying lowp vec2 coord;

// the texture
uniform sampler2D txr;

lowp float effectiveRadius( in lowp vec4 radii, in lowp vec2 point )
{    
    // aliases
    lowp float px = point.x * rectOfScreen.z;
    lowp float py = point.y * rectOfScreen.w;
    lowp float rx = rectOnScreen.x;
    lowp float ry = rectOnScreen.y;
    lowp float rw = rectOnScreen.z;
    lowp float rh = rectOnScreen.w;

    // predicates
    bool l = px >= rx && px < rx + rw / 2.0;
    bool r = px >= rx + rw / 2.0 && px < rx + rw;
    bool t = py >= ry && py < ry + rh / 2.0;
    bool b = py >= ry + rh / 2.0 && py < ry + rh;

    if ( t && l) return radii.x;
    if ( t && r) return radii.y;
    if ( b && r) return radii.z;
    if ( b && l) return radii.w;
    return 0.0;
}

// from https://iquilezles.org/articles/distfunctions
lowp float roundedBoxSDF(vec2 centerPosition, vec2 size, lowp float radius)
{
    return length( max( abs(centerPosition) - size + radius, 0.0) ) - radius;
}

// source: https://www.shadertoy.com/view/Xltfzj
void main()
{
    if(rectOpacity == 0.0)
    {
        return;
    }

    // Radius for blurring around the pixel
    vec2 blurRadius = blurSize / rectOnScreen.zw;

    // Pixel colour
    vec4 fragColor = texture2D(txr, coord);

    // Blur calculations
    const lowp float Pi = 6.28318530718; // constant for Pi*2
    for( float d = 0.0; d < Pi; d += Pi / blurDirections)
    {
        for(float i = 1.0 / blurQuality; i <= 1.0; i += 1.0 / blurQuality)
        {
            fragColor += texture2D( txr, coord + vec2(cos(d), sin(d)) * blurRadius * i);
        }
    }

    lowp float cornerRadius = effectiveRadius(rectCornerRadii, coord);
    lowp float edgeSoftness = 1.0f;
    lowp float distance = roundedBoxSDF(coord * rectOfScreen.zw - rectOnScreen.xy - rectOnScreen.zw * 0.5f, rectOnScreen.zw * 0.5f, cornerRadius);
    lowp float smoothedAlpha = 1.0f - smoothstep(0.0f, edgeSoftness * 2.0f, distance);

    // Output to screen
    fragColor /= blurQuality * blurDirections - 15.0;

    gl_FragColor = mix(texture2D( txr, coord), fragColor, smoothedAlpha) * rectOpacity;
}
