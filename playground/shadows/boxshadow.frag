/* 
	Heavily inspired by code from the kirigami project:
		Copyright 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
	    License: LGPL-2.0-or-later

	https://iquilezles.org/www/articles/distfunctions2d/distfunctions2d.htm
		Copyright: 2017 Inigo Quilez
		License: MIT
 */

uniform lowp float opacity;
uniform lowp float extent;
uniform lowp vec4 radius;
uniform lowp vec4 color;
uniform lowp vec2 offset;
uniform lowp vec2 aspect;

varying lowp vec2 coord;

lowp float effectiveRadius( in lowp vec4 radii, in lowp vec2 point )
{
	if ( point.x > 0.0 )
		return ( point.y > 0.0) ? radii.x : radii.y;
	else
		return ( point.y > 0.0) ? radii.z : radii.w;
}

bool isInside( in lowp vec2 point, in lowp vec2 size, in lowp vec4 radii )
{
    lowp float r = effectiveRadius( radii, point );
    lowp vec2 d = abs(point) - size + r;

    lowp float l = min( max(d.x, d.y), 0.0) + length( max(d, 0.0) ) - r;
	return l <= 0.0;
}

lowp float shadowAt(
	in lowp vec2 point, in lowp vec2 size, in lowp vec4 radii )
{
    lowp float r = effectiveRadius( radii, point );
    lowp vec2 d = abs(point) - size + r;

    return min( max(d.x, d.y), 0.0) + length( max(d, 0.0) ) - r;
}

void main()
{
	lowp vec4 col = vec4(0.0);

	if ( extent > 0.0 && opacity > 0.0 )
	{
		lowp float t = 1.0 + 2.0 * length( offset ) + extent;

		if ( !isInside( coord, aspect / t, radius / t) )
		{
			const lowp float minRadius = 0.05;
			lowp float e2 = 0.5 * extent;

			lowp vec4 f = minRadius / max( radius, minRadius );
			lowp vec4 r = radius + e2 * f;

			lowp float shadow = shadowAt(
				coord - 2.0 * offset / t, aspect / t, r / t);

			lowp float v = smoothstep( -e2, e2, shadow );

			col = mix( color, vec4(0.0), v ) * opacity;
		}
	}

    gl_FragColor = col; 
}
