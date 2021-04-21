/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

// See sdf.glsl for the SDF related functions.

// This shader renders a texture on top of a rectangle with rounded corners and
// a shadow below it.

uniform lowp float opacity;
uniform lowp float size;
uniform lowp vec4 radius;
uniform lowp vec4 color;
uniform lowp vec4 shadowColor;
uniform lowp vec2 offset;
uniform lowp vec2 aspect;
uniform sampler2D textureSource;

#ifdef CORE_PROFILE
in lowp vec2 uv;
out lowp vec4 out_color;
#else
varying lowp vec2 uv;
#define out_color gl_FragColor
#define texture texture2D
#endif

void main()
{
    lowp vec4 col = vec4(0.0);

    // Calculate the main rectangle distance field.
    lowp float rect = sdf_rounded_rectangle(uv, aspect, radius);

    // Sample the texture, then render it, without any alpha blending.
    lowp vec2 texture_uv = ((uv / aspect) + 1.0) / 2.0;
    lowp vec4 texture_color = vec4(texture(textureSource, texture_uv).rgb, 1.0);

    col = sdf_render(rect, col, texture_color);

    out_color = col * opacity;
}
