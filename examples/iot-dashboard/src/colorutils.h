/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QColor>
#include <QObject>
#include <QQuickItem>
#include <QJSValue>

/**
 * Utilities for processing items to obtain colors and information useful for
 * UIs that need to adjust to variable elements.
 */
class ColorUtils : public QObject
{
    Q_OBJECT
public:
    /**
     * Describes the contrast of an item.
     */
    enum Brightness {
        Dark, /**< The item is dark and requires a light foreground color to achieve readable contrast. */
        Light, /**< The item is light and requires a dark foreground color to achieve readable contrast. */
    };
    Q_ENUM(Brightness)

    explicit ColorUtils(QObject* parent = nullptr);

    /**
     * Returns whether a color is bright or dark.
     * 
     * @code{.qml}
     * import QtQuick 2.0
     * import org.kde.kirigami 2.12 as Kirigami
     *
     * Kirigami.Heading {
     *     text: {
     *         if (Kirigami.ColorUtils.brightnessForColor("pink") == Kirigami.ColorUtils.Light) {
     *             return "The color is light"
     *         } else {
     *             return "The color is dark"
     *         }
     *     }
     * }
     * @endcode
     * 
     * @since 5.69
     * @since org.kde.kirigami 2.12
     */
    Q_INVOKABLE ColorUtils::Brightness brightnessForColor(const QColor &color);

    /**
     * Returns the result of overlaying the foreground color on the background
     * color.
     * 
     * @param foreground The color to overlay on the background.
     * 
     * @param background The color to overlay the foreground on.
     * 
     * @code{.qml}
     * import QtQuick 2.0
     * import org.kde.kirigami 2.12 as Kirigami
     *
     * Rectangle {
     *     color: Kirigami.ColorUtils.alphaBlend(Qt.rgba(0, 0, 0, 0.5), Qt.rgba(1, 1, 1, 1))
     * }
     * @endcode
     * 
     * @since 5.69
     * @since org.kde.kirigami 2.12
     */
    Q_INVOKABLE QColor alphaBlend(const QColor &foreground, const QColor &background);

    /**
     * Returns a linearly interpolated color between color one and color two.
     * 
     * @param one The color to linearly interpolate from.
     * 
     * @param two The color to linearly interpolate to.
     * 
     * @param balance The balance between the two colors. 0.0 will return the
     * first color, 1.0 will return the second color. Values beyond these bounds
     * are valid, and will result in extrapolation.
     * 
     * @code{.qml}
     * import QtQuick 2.0
     * import org.kde.kirigami 2.12 as Kirigami
     *
     * Rectangle {
     *     color: Kirigami.ColorUtils.linearInterpolation("black", "white", 0.5)
     * }
     * @endcode
     * 
     * @since 5.69
     * @since org.kde.kirigami 2.12
     */
    Q_INVOKABLE QColor linearInterpolation(const QColor &one, const QColor &two, double balance);

    /**
     * Increases or decreases the properties of `color` by fixed amounts.
     * 
     * @param color The color to adjust.
     * 
     * @param adjustments The adjustments to apply to the color.
     * 
     * @note `value` and `lightness` are aliases for the same value.
     * 
     * @code{.js}
     * {
     *     red: null, // Range: -255 to 255
     *     green: null, // Range: -255 to 255
     *     blue: null, // Range: -255 to 255
     *     hue: null, // Range: -360 to 360
     *     saturation: null, // Range: -255 to 255
     *     value: null // Range: -255 to 255
     *     lightness: null, // Range: -255 to 255
     *     alpha: null, // Range: -255 to 255
     * }
     * @endcode
     * 
     * @warning It is an error to adjust both RGB and HSL properties.
     * 
     * @since 5.69
     * @since org.kde.kirigami 2.12
     */
    Q_INVOKABLE QColor adjustColor(const QColor &color, const QJSValue &adjustments);

    /**
     * Smoothly scales colors.
     * 
     * @param color The color to adjust.
     * 
     * @param adjustments The adjustments to apply to the color. Each value must
     * be between `-100.0` and `100.0`. This indicates how far the property should
     * be scaled from its original to the maximum if positive or to the minumum if
     * negative.
     * 
     * @note `value` and `lightness` are aliases for the same value.
     * 
     * @code{.js}
     * {
     *     red: null
     *     green: null
     *     blue: null
     *     saturation: null
     *     lightness: null
     *     value: null
     *     alpha: null
     * }
     * @endcode
     * 
     * @warning It is an error to scale both RGB and HSL properties.
     * 
     * @since 5.69
     * @since org.kde.kirigami 2.12
     */
    Q_INVOKABLE QColor scaleColor(const QColor &color, const QJSValue &adjustments);

    /**
     * Tint a color using a separate alpha value.
     *
     * This does the same as Qt.tint() except that rather than using the tint
     * color's alpha value, it uses a separate value that gets multiplied with
     * the tint color's alpha. This avoids needing to create a new color just to
     * adjust an alpha value.
     *
     * \param targetColor The color to tint.
     * \param tintColor The color to tint with.
     * \param alpha The amount of tinting to apply.
     *
     * \return The tinted color.
     *
     * \sa Qt.tint()
     */
    Q_INVOKABLE QColor tintWithAlpha(const QColor &targetColor, const QColor &tintColor, double alpha);

    /**
     * Returns the CIELAB chroma of the given color.
     *
     * CIELAB chroma may give a better quantification of how vibrant a color is compared to HSV saturation.
     *
     * \sa https://en.wikipedia.org/wiki/Colorfulness
     * \sa https://en.wikipedia.org/wiki/CIELAB_color_space
     */
    Q_INVOKABLE static qreal chroma(const QColor &color);

    struct LabColor {
        qreal l = 0;
        qreal a = 0;
        qreal b = 0;
    };

    // Not for QML, returns the comvertion from srgb of a QColor and Lab colorspace
    static ColorUtils::LabColor colorToLab(const QColor &color);
};
