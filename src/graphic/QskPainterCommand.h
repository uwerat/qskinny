/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PAINTER_COMMAND_H
#define QSK_PAINTER_COMMAND_H

#include "QskGlobal.h"

#include <qimage.h>
#include <qpaintengine.h>
#include <qpainterpath.h>
#include <qpixmap.h>

class QSK_EXPORT QskPainterCommand
{
  public:
    enum Type : qint8
    {
        Invalid = -1,

        Path,
        Pixmap,
        Image,

        State
    };

    struct PixmapData
    {
        QRectF rect;
        QPixmap pixmap;
        QRectF subRect;
    };

    struct ImageData
    {
        QRectF rect;
        QImage image;
        QRectF subRect;
        Qt::ImageConversionFlags flags;
    };

    struct StateData
    {
        QPaintEngine::DirtyFlags flags;

        QPen pen;
        QBrush brush;
        QPointF brushOrigin;
        QBrush backgroundBrush;
        Qt::BGMode backgroundMode = Qt::TransparentMode;
        QFont font;
        QTransform transform;

        Qt::ClipOperation clipOperation = Qt::NoClip;
        QRegion clipRegion;
        QPainterPath clipPath;
        bool isClipEnabled = false;

        QPainter::RenderHints renderHints;
        QPainter::CompositionMode compositionMode = QPainter::CompositionMode_SourceOver;
        qreal opacity;
    };

    constexpr QskPainterCommand() noexcept;
    QskPainterCommand( const QskPainterCommand& );

    explicit QskPainterCommand( const QPainterPath& );

    QskPainterCommand( const QRectF& rect,
        const QPixmap&, const QRectF& subRect );

    QskPainterCommand( const QRectF& rect,
        const QImage&, const QRectF& subRect,
        Qt::ImageConversionFlags );

    explicit QskPainterCommand( const QskPainterCommand::StateData& data );
    explicit QskPainterCommand( const QPaintEngineState& );

    ~QskPainterCommand();

    QskPainterCommand& operator=( const QskPainterCommand& );

    bool operator==( const QskPainterCommand& other ) const noexcept;
    bool operator!=( const QskPainterCommand& other ) const noexcept;

    Type type() const noexcept;

    QPainterPath* path() noexcept;
    const QPainterPath* path() const noexcept;

    PixmapData* pixmapData() noexcept;
    const PixmapData* pixmapData() const noexcept;

    ImageData* imageData() noexcept;
    const ImageData* imageData() const noexcept;

    StateData* stateData() noexcept;
    const StateData* stateData() const noexcept;

  private:
    void copy( const QskPainterCommand& );
    void reset();

    Type m_type;

    union
    {
        QPainterPath* m_path;
        PixmapData* m_pixmapData;
        ImageData* m_imageData;
        StateData* m_stateData;
    };
};

constexpr inline QskPainterCommand::QskPainterCommand() noexcept
    : m_type( Invalid )
    , m_path( nullptr )
{
}

inline bool QskPainterCommand::operator!=( const QskPainterCommand& other ) const noexcept
{
    return !( *this == other );
}

inline QskPainterCommand::Type QskPainterCommand::type() const noexcept
{
    return m_type;
}

inline const QPainterPath* QskPainterCommand::path() const noexcept
{
    return m_path;
}

inline const QskPainterCommand::PixmapData*
QskPainterCommand::pixmapData() const noexcept
{
    return m_pixmapData;
}

inline const QskPainterCommand::ImageData*
QskPainterCommand::imageData() const noexcept
{
    return m_imageData;
}

inline const QskPainterCommand::StateData*
QskPainterCommand::stateData() const noexcept
{
    return m_stateData;
}

#endif
