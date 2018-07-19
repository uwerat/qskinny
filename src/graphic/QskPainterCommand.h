/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAINTER_COMMAND_H
#define QSK_PAINTER_COMMAND_H

#include "QskGlobal.h"
#include <qpaintengine.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainterpath.h>

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
    class StateData
    {
    public:
        StateData():
            backgroundMode( Qt::TransparentMode ),
            clipOperation( Qt::NoClip ),
            isClipEnabled( false ),
            compositionMode( QPainter::CompositionMode_SourceOver )
        {
        }

        QPaintEngine::DirtyFlags flags;

        QPen pen;
        QBrush brush;
        QPointF brushOrigin;
        QBrush backgroundBrush;
        Qt::BGMode backgroundMode;
        QFont font;
        QMatrix matrix;
        QTransform transform;

        Qt::ClipOperation clipOperation;
        QRegion clipRegion;
        QPainterPath clipPath;
        bool isClipEnabled;

        QPainter::RenderHints renderHints;
        QPainter::CompositionMode compositionMode;
        qreal opacity;
    };

    QskPainterCommand();
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

    bool operator==( const QskPainterCommand& other ) const;
    bool operator!=( const QskPainterCommand& other ) const;


    Type type() const;

    QPainterPath* path();
    const QPainterPath* path() const;

    PixmapData* pixmapData();
    const PixmapData* pixmapData() const;

    ImageData* imageData();
    const ImageData* imageData() const;

    StateData* stateData();
    const StateData* stateData() const;

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

inline bool QskPainterCommand::operator!=( const QskPainterCommand& other ) const
{
    return !( *this == other );
}

//! \return Type of the command
inline QskPainterCommand::Type QskPainterCommand::type() const
{
    return m_type;
}

//! \return Painter path to be painted
inline const QPainterPath* QskPainterCommand::path() const
{
    return m_path;
}

//! \return Attributes how to paint a QPixmap
inline const QskPainterCommand::PixmapData*
QskPainterCommand::pixmapData() const
{
    return m_pixmapData;
}

//! \return Attributes how to paint a QImage
inline const QskPainterCommand::ImageData*
QskPainterCommand::imageData() const
{
    return m_imageData;
}

//! \return Attributes of a state change
inline const QskPainterCommand::StateData*
QskPainterCommand::stateData() const
{
    return m_stateData;
}

#endif
