/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

// See sdf.glsl for the SDF related functions.

// This shader renders a rectangle with rounded corners and a shadow below it.
// In addition it renders a border around it.

uniform lowp float opacity;
uniform lowp float size;
uniform lowp vec4 radius;
uniform lowp vec4 color;
uniform lowp vec4 shadowColor;
uniform lowp vec2 offset;
uniform lowp vec2 aspect;
uniform lowp float borderWidth;
uniform lowp vec4 borderColor;
uniform sampler2D textureSource;

#ifdef CORE_PROFILE
in lowp vec2 uv;
out lowp vec4 out_color;
#else
varying lowp vec2 uv;
#define out_color gl_FragColor
#define texture texture2D
#endif

const lowp float minimum_shadow_radius = 0.05;

void main()
{
    lowp vec4 col = vec4(0.0);

    // Calculate the outer rectangle distance field.
    lowp float outer_rect = sdf_rounded_rectangle(uv, aspect, radius);

    // Render it, cancelling out any alpha components.
    col = sdf_render(outer_rect, col, vec4(borderColor.rgb, 1.0));

    // Inner rectangle distance field equals outer reduced by twice the border width
    lowp float inner_rect = outer_rect + borderWidth * 2.0;

    // Sample the texture, then render it on top of the background color.
    lowp vec2 texture_uv = ((uv / aspect) + 1.0) / 2.0;
    lowp vec4 texture_color = vec4(texture(textureSource, texture_uv).rgb, 1.0);
    col = sdf_render(inner_rect, col, texture_color, texture_color.a, sdf_default_smoothing / 2.0);

    out_color = col * opacity;
}
