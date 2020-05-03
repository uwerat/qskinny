/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPainterCommand.h"

QskPainterCommand::QskPainterCommand( const QPainterPath& path )
    : m_type( Path )
{
    m_path = new QPainterPath( path );
}

QskPainterCommand::QskPainterCommand( const QRectF& rect,
        const QPixmap& pixmap, const QRectF& subRect )
    : m_type( Pixmap )
{
    m_pixmapData = new PixmapData();
    m_pixmapData->rect = rect;
    m_pixmapData->pixmap = pixmap;
    m_pixmapData->subRect = subRect;
}

QskPainterCommand::QskPainterCommand( const QRectF& rect,
        const QImage& image, const QRectF& subRect, Qt::ImageConversionFlags flags )
    : m_type( Image )
{
    m_imageData = new ImageData();
    m_imageData->rect = rect;
    m_imageData->image = image;
    m_imageData->subRect = subRect;
    m_imageData->flags = flags;
}

QskPainterCommand::QskPainterCommand( const QskPainterCommand::StateData& data )
    : m_type( State )
{
    m_stateData = new StateData( data );
}

QskPainterCommand::QskPainterCommand( const QPaintEngineState& state )
    : m_type( State )
{
    m_stateData = new StateData();

    m_stateData->flags = state.state();

    if ( m_stateData->flags & QPaintEngine::DirtyPen )
        m_stateData->pen = state.pen();

    if ( m_stateData->flags & QPaintEngine::DirtyBrush )
        m_stateData->brush = state.brush();

    if ( m_stateData->flags & QPaintEngine::DirtyBrushOrigin )
        m_stateData->brushOrigin = state.brushOrigin();

    if ( m_stateData->flags & QPaintEngine::DirtyFont )
        m_stateData->font = state.font();

    if ( m_stateData->flags & QPaintEngine::DirtyBackground )
    {
        m_stateData->backgroundMode = state.backgroundMode();
        m_stateData->backgroundBrush = state.backgroundBrush();
    }

    if ( m_stateData->flags & QPaintEngine::DirtyTransform )
        m_stateData->transform = state.transform();

    if ( m_stateData->flags & QPaintEngine::DirtyClipEnabled )
        m_stateData->isClipEnabled = state.isClipEnabled();

    if ( m_stateData->flags & QPaintEngine::DirtyClipRegion )
    {
        m_stateData->clipRegion = state.clipRegion();
        m_stateData->clipOperation = state.clipOperation();
    }

    if ( m_stateData->flags & QPaintEngine::DirtyClipPath )
    {
        m_stateData->clipPath = state.clipPath();
        m_stateData->clipOperation = state.clipOperation();
    }

    if ( m_stateData->flags & QPaintEngine::DirtyHints )
        m_stateData->renderHints = state.renderHints();

    if ( m_stateData->flags & QPaintEngine::DirtyCompositionMode )
        m_stateData->compositionMode = state.compositionMode();

    if ( m_stateData->flags & QPaintEngine::DirtyOpacity )
        m_stateData->opacity = state.opacity();
}

QskPainterCommand::QskPainterCommand( const QskPainterCommand& other )
{
    copy( other );
}

QskPainterCommand::~QskPainterCommand()
{
    reset();
}

QskPainterCommand& QskPainterCommand::operator=( const QskPainterCommand& other )
{
    reset();
    copy( other );

    return *this;
}

bool QskPainterCommand::operator==( const QskPainterCommand& other ) const noexcept
{
    if ( m_type != other.m_type )
        return false;

    switch ( m_type )
    {
        case Path:
        {
            return ( m_path == other.m_path );
        }
        case Pixmap:
        {
            return ( m_pixmapData == other.m_pixmapData );
        }
        case Image:
        {
            return ( m_imageData == other.m_imageData );
        }
        case State:
        {
            const StateData& sd = *m_stateData;
            const StateData& osd = *other.m_stateData;

            if ( sd.flags != osd.flags )
                return false;

            if ( sd.flags & QPaintEngine::DirtyPen )
            {
                if ( sd.pen != osd.pen )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyBrush )
            {
                if ( sd.brush != osd.brush )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyBrushOrigin )
            {
                if ( sd.brushOrigin != osd.brushOrigin )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyFont )
            {
                if ( sd.font != osd.font )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyBackground )
            {
                if ( sd.backgroundMode != osd.backgroundMode ||
                    sd.backgroundBrush != osd.backgroundBrush )
                {
                    return false;
                }
            }

            if ( sd.flags & QPaintEngine::DirtyTransform )
            {
                if ( sd.transform != osd.transform )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyClipEnabled )
            {
                if ( sd.isClipEnabled != osd.isClipEnabled )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyClipRegion )
            {
                if ( sd.clipRegion != osd.clipRegion ||
                    sd.clipOperation != osd.clipOperation )
                {
                    return false;
                }
            }

            if ( sd.flags & QPaintEngine::DirtyClipPath )
            {
                if ( sd.clipPath != osd.clipPath ||
                    sd.clipOperation != osd.clipOperation )
                {
                    return false;
                }
            }

            if ( sd.flags & QPaintEngine::DirtyHints )
            {
                if ( sd.renderHints != osd.renderHints )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyCompositionMode )
            {
                if ( sd.compositionMode != osd.compositionMode )
                    return false;
            }

            if ( sd.flags & QPaintEngine::DirtyOpacity )
            {
                if ( sd.opacity != osd.opacity )
                    return false;
            }

            return true;
        }
        default:
            break;
    }

    return true;
}

void QskPainterCommand::copy( const QskPainterCommand& other )
{
    m_type = other.m_type;

    switch ( other.m_type )
    {
        case Path:
        {
            m_path = new QPainterPath( *other.m_path );
            break;
        }
        case Pixmap:
        {
            m_pixmapData = new PixmapData( *other.m_pixmapData );
            break;
        }
        case Image:
        {
            m_imageData = new ImageData( *other.m_imageData );
            break;
        }
        case State:
        {
            m_stateData = new StateData( *other.m_stateData );
            break;
        }
        default:
            break;
    }
}

void QskPainterCommand::reset()
{
    switch ( m_type )
    {
        case Path:
        {
            delete m_path;
            break;
        }
        case Pixmap:
        {
            delete m_pixmapData;
            break;
        }
        case Image:
        {
            delete m_imageData;
            break;
        }
        case State:
        {
            delete m_stateData;
            break;
        }
        default:
            break;
    }

    m_type = Invalid;
}

QPainterPath* QskPainterCommand::path() noexcept
{
    return m_path;
}

QskPainterCommand::PixmapData* QskPainterCommand::pixmapData() noexcept
{
    return m_pixmapData;
}

QskPainterCommand::ImageData* QskPainterCommand::imageData() noexcept
{
    return m_imageData;
}

QskPainterCommand::StateData* QskPainterCommand::stateData() noexcept
{
    return m_stateData;
}
