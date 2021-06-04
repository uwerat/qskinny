/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

// See sdf.glsl for the SDF related functions.

// This is a version of shadowedrectangle.frag meant for very low power hardware
// (PinePhone). It does not render a shadow and does not do alpha blending.

uniform lowp float opacity;
uniform lowp float size;
uniform lowp vec4 radius;
uniform lowp vec4 color;
uniform lowp vec4 shadowColor;
uniform lowp vec2 offset;
uniform lowp vec2 aspect;

#ifdef CORE_PROFILE
in lowp vec2 uv;
out lowp vec4 out_color;
#else
varying lowp vec2 uv;
#define out_color gl_FragColor
#endif

void main()
{
    lowp vec4 col = vec4(0.0);

    // Calculate the main rectangle distance field.
    lowp float rect = sdf_rounded_rectangle(uv, aspect, radius);

    // Render it, cancelling out any alpha component.
    col = sdf_render(rect, col, vec4(color.rgb, 1.0));

    out_color = col * opacity;
}
