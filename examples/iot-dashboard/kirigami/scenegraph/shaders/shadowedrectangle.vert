/*
 *  SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

uniform highp mat4 matrix;
uniform lowp vec2 aspect;

#ifdef CORE_PROFILE
in highp vec4 in_vertex;
in mediump vec2 in_uv;
out mediump vec2 uv;
#else
attribute highp vec4 in_vertex;
attribute mediump vec2 in_uv;
varying mediump vec2 uv;
#endif

void main() {
    uv = (-1.0 + 2.0 * in_uv) * aspect;
    gl_Position = matrix * in_vertex;
}
