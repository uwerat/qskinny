/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
    //! Type of the paint command
    enum Type
    {
        //! Invalid command
        Invalid = -1,

        //! Draw a QPainterPath
        Path,

        //! Draw a QPixmap
        Pixmap,

        //! Draw a QImage
        Image,

        //! QPainter state change
        State
    };

    //! Attributes how to paint a QPixmap
    struct PixmapData
    {
        QRectF rect;
        QPixmap pixmap;
        QRectF subRect;
    };

    //! Attributes how to paint a QImage
    struct ImageData
    {
        QRectF rect;
        QImage image;
        QRectF subRect;
        Qt::ImageConversionFlags flags;
    };

    //! Attributes of a state change
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

//! \return Type of the command
inline QskPainterCommand::Type QskPainterCommand::type() const noexcept
{
    return m_type;
}

//! \return Painter path to be painted
inline const QPainterPath* QskPainterCommand::path() const noexcept
{
    return m_path;
}

//! \return Attributes how to paint a QPixmap
inline const QskPainterCommand::PixmapData*
QskPainterCommand::pixmapData() const noexcept
{
    return m_pixmapData;
}

//! \return Attributes how to paint a QImage
inline const QskPainterCommand::ImageData*
QskPainterCommand::imageData() const noexcept
{
    return m_imageData;
}

//! \return Attributes of a state change
inline const QskPainterCommand::StateData*
QskPainterCommand::stateData() const noexcept
{
    return m_stateData;
}

#endif
