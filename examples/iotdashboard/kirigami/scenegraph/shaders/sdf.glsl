// SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2017 Inigo Quilez
//
// SPDX-License-Identifier: MIT
//
// This file is based on
// https://iquilezles.org/www/articles/distfunctions2d/distfunctions2d.htm

//if not GLES
// include "desktop_header.glsl"
//else
// include "es_header.glsl"

// A maximum point count to be used for sdf_polygon input arrays.
// Unfortunately even function inputs require a fixed size at declaration time
// for arrays, unless we were to use OpenGL 4.5.
// Since the polygon is most likely to be defined in a uniform, this should be
// at least less than MAX_FRAGMENT_UNIFORM_COMPONENTS / 2 (since we need vec2).
#define SDF_POLYGON_MAX_POINT_COUNT 400

/*********************************
    Shapes
*********************************/

// Distance field for a circle.
//
// \param point A point on the distance field.
// \param radius The radius of the circle.
//
// \return The signed distance from point to the circle. If negative, point is
//         inside the circle.
lowp float sdf_circle(in lowp vec2 point, in lowp float radius)
{
    return length(point) - radius;
}

// Distance field for a triangle.
//
// \param point A point on the distance field.
// \param p0 The first vertex of the triangle.
// \param p0 The second vertex of the triangle.
// \param p0 The third vertex of the triangle.
//
// \note The ordering of the three vertices does not matter.
//
// \return The signed distance from point to triangle. If negative, point is
//         inside the triangle.
lowp float sdf_triangle(in lowp vec2 point, in lowp vec2 p0, in lowp vec2 p1, in lowp vec2 p2)
{
    lowp vec2 e0 = p1 - p0;
    lowp vec2 e1 = p2 - p1;
    lowp vec2 e2 = p0 - p2;

    lowp vec2 v0 = point - p0;
    lowp vec2 v1 = point - p1;
    lowp vec2 v2 = point - p2;

    lowp vec2 pq0 = v0 - e0 * clamp( dot(v0, e0) / dot(e0, e0), 0.0, 1.0 );
    lowp vec2 pq1 = v1 - e1 * clamp( dot(v1, e1) / dot(e1, e1), 0.0, 1.0 );
    lowp vec2 pq2 = v2 - e2 * clamp( dot(v2, e2) / dot(e2, e2), 0.0, 1.0 );

    lowp float s = sign( e0.x*e2.y - e0.y*e2.x );
    lowp vec2 d = min(min(vec2(dot(pq0,pq0), s*(v0.x*e0.y-v0.y*e0.x)),
                          vec2(dot(pq1,pq1), s*(v1.x*e1.y-v1.y*e1.x))),
                          vec2(dot(pq2,pq2), s*(v2.x*e2.y-v2.y*e2.x)));

    return -sqrt(d.x)*sign(d.y);
}

// Distance field for a rectangle.
//
// \param point A point on the distance field.
// \param rect A vec2 with the size of the rectangle.
//
// \return The signed distance from point to rectangle. If negative, point is
//         inside the rectangle.
lowp float sdf_rectangle(in lowp vec2 point, in lowp vec2 rect)
{
    lowp vec2 d = abs(point) - rect;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

// Distance field for a rectangle with rounded corners.
//
// \param point The point to calculate the distance of.
// \param rect The rectangle to calculate the distance of.
// \param radius A vec4 with the radius of each corner. Order is top right, bottom right, top left, bottom left.
//
// \return The signed distance from point to rectangle. If negative, point is
//         inside the rectangle.
lowp float sdf_rounded_rectangle(in lowp vec2 point, in lowp vec2 rect, in lowp vec4 radius)
{
    radius.xy = (point.x > 0.0) ? radius.xy : radius.zw;
    radius.x = (point.y > 0.0) ? radius.x : radius.y;
    lowp vec2 d = abs(point) - rect + radius.x;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius.x;
}

/*********************
    Operators
*********************/

// Convert a distance field to an annular (hollow) distance field.
//
// \param sdf The result of an sdf shape to convert.
// \param thickness The thickness of the resulting shape.
//
// \return The value of sdf modified to an annular shape.
lowp float sdf_annular(in lowp float sdf, in lowp float thickness)
{
    return abs(sdf) - thickness;
}

// Union two sdf shapes together.
//
// \param sdf1 The first sdf shape.
// \param sdf2 The second sdf shape.
//
// \return The union of sdf1 and sdf2, that is, the distance to both sdf1 and
//         sdf2.
lowp float sdf_union(in lowp float sdf1, in lowp float sdf2)
{
    return min(sdf1, sdf2);
}

// Subtract two sdf shapes.
//
// \param sdf1 The first sdf shape.
// \param sdf2 The second sdf shape.
//
// \return sdf1 with sdf2 subtracted from it.
lowp float sdf_subtract(in lowp float sdf1, in lowp float sdf2)
{
    return max(sdf1, -sdf2);
}

// Intersect two sdf shapes.
//
// \param sdf1 The first sdf shape.
// \param sdf2 The second sdf shape.
//
// \return The intersection between sdf1 and sdf2, that is, the area where both
//         sdf1 and sdf2 provide the same distance value.
lowp float sdf_intersect(in lowp float sdf1, in lowp float sdf2)
{
    return max(sdf1, sdf2);
}

// Smoothly intersect two sdf shapes.
//
// \param sdf1 The first sdf shape.
// \param sdf2 The second sdf shape.
// \param smoothing The amount of smoothing to apply.
//
// \return A smoothed version of the intersect operation.
lowp float sdf_intersect_smooth(in lowp float sdf1, in lowp float sdf2, in lowp float smoothing)
{
    lowp float h = clamp(0.5 - 0.5 * (sdf1 - sdf2) / smoothing, 0.0, 1.0);
    return mix(sdf1, sdf2, h) + smoothing * h * (1.0 - h);
}

// Round an sdf shape.
//
// \param sdf The sdf shape to round.
// \param amount The amount of rounding to apply.
//
// \return The rounded shape of sdf.
//         Note that rounding happens by basically selecting an isoline of sdf,
//         therefore, the resulting shape may be larger than the input shape.
lowp float sdf_round(in lowp float sdf, in lowp float amount)
{
    return sdf - amount;
}

// Convert an sdf shape to an outline of its shape.
//
// \param sdf The sdf shape to turn into an outline.
//
// \return The outline of sdf.
lowp float sdf_outline(in lowp float sdf)
{
    return abs(sdf);
}

/********************
    Convenience
********************/

// A constant to represent a "null" value of an sdf.
//
// Since 0 is a point exactly on the outline of an sdf shape, and negative
// values are inside the shape, this uses a very large positive constant to
// indicate a value that is really far away from the actual sdf shape.
const lowp float sdf_null = 99999.0;

// A constant for a default level of smoothing when rendering an sdf.
//
// This
const lowp float sdf_default_smoothing = 0.625;

// Render an sdf shape alpha-blended onto an existing color.
//
// This is an overload of sdf_render(float, vec4, vec4) that allows specifying a
// blending amount and a smoothing amount.
//
// \param alpha The alpha to use for blending.
// \param smoothing The amount of smoothing to apply to the sdf.
//
lowp vec4 sdf_render(in lowp float sdf, in lowp vec4 sourceColor, in lowp vec4 sdfColor, in lowp float alpha, in lowp float smoothing)
{
   lowp float g = fwidth(sdf);
   return mix(sourceColor, sdfColor, alpha * (1.0 - smoothstep(-smoothing * g, smoothing * g, sdf)));
}

// Render an sdf shape.
//
// This will render the sdf shape on top of whatever source color is input,
// making sure to apply smoothing if desired.
//
// \param sdf The sdf shape to render.
// \param sourceColor The source color to render on top of.
// \param sdfColor The color to use for rendering the sdf shape.
//
// \return sourceColor with the sdf shape rendered on top.
lowp vec4 sdf_render(in lowp float sdf, in lowp vec4 sourceColor, in lowp vec4 sdfColor)
{
    return sdf_render(sdf, sourceColor, sdfColor, 1.0, sdf_default_smoothing);
}

// Render an sdf shape.
//
// This is an overload of sdf_render(float, vec4, vec4) that allows specifying a
// smoothing amount.
//
// \param smoothing The amount of smoothing to apply to the sdf.
//
lowp vec4 sdf_render(in lowp float sdf, in lowp vec4 sourceColor, in lowp vec4 sdfColor, in lowp float smoothing)
{
    return sdf_render(sdf, sourceColor, sdfColor, 1.0, smoothing);
}
