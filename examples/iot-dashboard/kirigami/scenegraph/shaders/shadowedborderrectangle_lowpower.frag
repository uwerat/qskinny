/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

// See sdf.glsl for the SDF related functions.

// This is a version of shadowedborderrectangle.frag for extremely low powered
// hardware (PinePhone). It does not draw a shadow and also eliminates alpha
// blending.

uniform lowp float opacity;
uniform lowp float size;
uniform lowp vec4 radius;
uniform lowp vec4 color;
uniform lowp vec4 shadowColor;
uniform lowp vec2 offset;
uniform lowp vec2 aspect;
uniform lowp float borderWidth;
uniform lowp vec4 borderColor;

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

    // Calculate the outer rectangle distance field.
    lowp float outer_rect = sdf_rounded_rectangle(uv, aspect, radius);

    col = sdf_render(outer_rect, col, vec4(borderColor.rgb, 1.0));

    // The inner distance field is the outer reduced by border width.
    lowp float inner_rect = outer_rect + borderWidth * 2.0;

    col = sdf_render(inner_rect, col, vec4(color.rgb, 1.0));

    out_color = col * opacity;
}
