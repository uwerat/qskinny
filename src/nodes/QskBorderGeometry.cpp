/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBorderGeometry.h"

namespace
{
    static QSGGeometry::Attribute attributeData[] =
    {
        QSGGeometry::Attribute::create( 0, 2, GL_FLOAT, true ),
        QSGGeometry::Attribute::create( 1, 2, GL_FLOAT, false ),
        QSGGeometry::Attribute::create( 2, 4, GL_UNSIGNED_BYTE, false ),
        QSGGeometry::Attribute::create( 3, 4, GL_UNSIGNED_BYTE, false )
    };

    struct TexturePoint : public QSGGeometry::TexturedPoint2D
    {
        QRgb background;
        QRgb foreground;

        static const QSGGeometry::AttributeSet attributes;
    };

    const QSGGeometry::AttributeSet TexturePoint::attributes =
    {
        sizeof( attributeData ) / sizeof( attributeData[0] ),
        sizeof( TexturePoint ),
        attributeData
    };

    class Quad
    {
    public:
        void setRect( float x, float y, float width, float height )
        {
            topLeft.x = bottomLeft.x = x;
            topLeft.y = topRight.y = y;
            topRight.x = bottomRight.x = x + width;
            bottomLeft.y = bottomRight.y = y + height;
        }

        void setTexture( float tx, float ty, float tw, float th )
        {
            topLeft.tx = bottomLeft.tx = tx;
            topLeft.ty = topRight.ty = ty;
            topRight.tx = bottomRight.tx = tx + tw;
            bottomRight.ty = bottomLeft.ty = ty + th;
        }

        TexturePoint& topLeft;
        TexturePoint& topRight;
        TexturePoint& bottomLeft;
        TexturePoint& bottomRight;
    };

    inline Quad quadAt( void* vertexData, size_t col, size_t row )
    {
        TexturePoint* points = reinterpret_cast< TexturePoint* >( vertexData );

        const int i = row * 4 + col;
        return { points[i], points[i + 1], points[i + 4], points[i + 5] };
    }
}

QskBorderGeometry::QskBorderGeometry():
    QSGGeometry( TexturePoint::attributes, 16, 28 ),
    m_scaleX( 1.0 ),
    m_scaleY( 1.0 )
{
    setVertexDataPattern( QSGGeometry::StaticPattern );
    setIndexDataPattern( QSGGeometry::StaticPattern );

    initIndex();
}

QskBorderGeometry::~QskBorderGeometry()
{
}

void QskBorderGeometry::setBorder( const QRectF& rect, const QMarginsF& margins,
    const QRectF& textureCoordinates, const QSizeF& textureSize )
{
    if ( textureCoordinates.isEmpty() || textureSize.isEmpty() || rect.isEmpty() )
        return;

    const float sx = textureCoordinates.width() / textureSize.width();
    const float sy = textureCoordinates.height() / textureSize.height();

    if ( ( m_rect == rect ) && ( margins == m_border )
         && ( m_textureCoordinates == textureCoordinates )
         && ( sx == m_scaleX ) && ( sy == m_scaleY ) )
    {
        return;
    }

    m_rect = rect;
    m_border = margins;
    m_textureCoordinates = textureCoordinates;
    m_scaleX = sx;
    m_scaleY = sy;

    QMarginsF border = m_border;

    // If the rect's dimension is less than the border, truncate the border by equal amounts
    if ( m_rect.width() < border.left() + border.right() )
    {
        const auto truncate = ( border.left() + border.right() - m_rect.width() ) * 0.5f;
        border.setLeft( border.left() - truncate );
        border.setRight( border.right() - truncate );
    }

    if ( m_rect.height() < border.top() + border.bottom() )
    {
        const auto truncate = ( border.top() + border.bottom() - m_rect.height() ) * 0.5f;
        border.setTop( border.top() - truncate );
        border.setBottom( border.bottom() - truncate );
    }

    const auto borderLeft = border.left() * m_scaleX;
    const auto borderTop = border.top() * m_scaleY;
    const auto borderRight = border.right() * m_scaleX;
    const auto borderBottom = border.bottom() * m_scaleY;

    const float right = qMax( m_rect.left(), m_rect.right() - border.right() );
    const float borderInnerRight = m_textureCoordinates.right() - borderRight;
    const float bottom = qMax( m_rect.top(), m_rect.bottom() - border.bottom() );
    const float borderInnerBottom = m_textureCoordinates.bottom() - borderBottom;

    {
        Quad topLeft = quadAt( vertexData(), 0, 0 );
        topLeft.setRect( m_rect.left(), m_rect.top(), border.left(), border.top() );
        topLeft.setTexture( m_textureCoordinates.left(), m_textureCoordinates.top(),
            borderLeft, borderTop );
    }

    {
        Quad topRight = quadAt( vertexData(), 2, 0 );
        topRight.setRect( right, m_rect.top(), border.right(), border.top() );
        topRight.setTexture( borderInnerRight, m_textureCoordinates.top(), borderRight, borderTop );
    }

    {
        Quad bottomLeft = quadAt( vertexData(), 0, 2 );
        bottomLeft.setRect( m_rect.left(), bottom, border.left(), border.bottom() );
        bottomLeft.setTexture( m_textureCoordinates.left(), borderInnerBottom, borderLeft, borderBottom );
    }

    {
        Quad bottomRight = quadAt( vertexData(), 2, 2 );
        bottomRight.setRect( right, bottom, border.right(), border.bottom() );
        bottomRight.setTexture( borderInnerRight, borderInnerBottom, borderRight, borderBottom );
    }

    markVertexDataDirty();
}

void QskBorderGeometry::initIndex()
{
    quint16* indexData = indexDataAsUShort();

    constexpr size_t stride = ( 3 + 1 ) * 2;

    size_t offset = 0;
    for ( int i = 0; i < indexCount(); ++i )
    {
        auto n = i - offset;
        indexData[i] = static_cast< quint16 >( ( n % 2 ? ( n + stride ) : ( n + 1 ) ) / 2 );

        // Collapse triangles and move to beginning of row
        if ( i && ( ( ( i + 1 - offset ) % stride ) == 0 ) && i < indexCount() - 1 )
        {
            indexData[i + 1] = indexData[i];
            indexData[i + 2] = indexData[i + 3] = static_cast< quint16 >( ( n + 1 ) / 2 );
            i += 3;
            offset += 2;
        }
    }

    markIndexDataDirty();
}

void QskBorderGeometry::setEdgeBackground(
    Qt::Edge edge, QRgb backgroundIn, QRgb backgroundOut )
{
    backgroundIn = qPremultiply( backgroundIn );
    backgroundOut = qPremultiply( backgroundOut );

    if ( edge == Qt::LeftEdge )
    {
        auto quad = quadAt( vertexData(), 0, 1 );

        quad.topLeft.background = backgroundIn;
        quad.topRight.background = backgroundIn;
        quad.bottomLeft.background = backgroundOut;
        quad.bottomRight.background = backgroundOut;
    }
    else if ( edge == Qt::TopEdge )
    {
        auto quad = quadAt( vertexData(), 1, 0 );

        quad.topLeft.background = backgroundIn;
        quad.bottomLeft.background = backgroundIn;
        quad.topRight.background = backgroundOut;
        quad.bottomRight.background = backgroundOut;

        quadAt( vertexData(), 0, 0 ).topLeft.background = backgroundIn;
        quadAt( vertexData(), 2, 0 ).topRight.background = backgroundOut;
    }
    else if ( edge == Qt::RightEdge )
    {
        auto quad = quadAt( vertexData(), 2, 1 );

        quad.topLeft.background = backgroundIn;
        quad.topRight.background = backgroundIn;
        quad.bottomLeft.background = backgroundOut;
        quad.bottomRight.background = backgroundOut;
    }
    else if ( edge == Qt::BottomEdge )
    {
        auto quad = quadAt( vertexData(), 1, 2 );

        quad.topLeft.background = backgroundIn;
        quad.bottomLeft.background = backgroundIn;
        quad.topRight.background = backgroundOut;
        quad.bottomRight.background = backgroundOut;

        quadAt( vertexData(), 0, 2 ).bottomLeft.background = backgroundIn;
        quadAt( vertexData(), 2, 2 ).bottomRight.background = backgroundOut;
    }

    markVertexDataDirty();
}

void QskBorderGeometry::setEdgeForeground( Qt::Edge edge, QRgb foreground )
{
    foreground = qPremultiply( foreground );

    if ( edge == Qt::LeftEdge )
    {
        auto quad = quadAt( vertexData(), 0, 1 );

        quad.topLeft.foreground = foreground;
        quad.topRight.foreground = foreground;
        quad.bottomLeft.foreground = foreground;
        quad.bottomRight.foreground = foreground;
    }
    else if ( edge == Qt::TopEdge )
    {
        auto quad = quadAt( vertexData(), 1, 0 );

        quad.topLeft.foreground = foreground;
        quad.bottomLeft.foreground = foreground;
        quad.topRight.foreground = foreground;
        quad.bottomRight.foreground = foreground;

        quadAt( vertexData(), 0, 0 ).topLeft.foreground = foreground;
        quadAt( vertexData(), 2, 0 ).topRight.foreground = foreground;
    }
    else if ( edge == Qt::RightEdge )
    {
        auto quad = quadAt( vertexData(), 2, 1 );

        quad.topLeft.foreground = foreground;
        quad.topRight.foreground = foreground;
        quad.bottomLeft.foreground = foreground;
        quad.bottomRight.foreground = foreground;
    }
    else if ( edge == Qt::BottomEdge )
    {
        auto quad = quadAt( vertexData(), 1, 2 );

        quad.topLeft.foreground = foreground;
        quad.bottomLeft.foreground = foreground;
        quad.topRight.foreground = foreground;
        quad.bottomRight.foreground = foreground;

        quadAt( vertexData(), 0, 2 ).bottomLeft.foreground = foreground;
        quadAt( vertexData(), 2, 2 ).bottomRight.foreground = foreground;
    }

    markVertexDataDirty();
}
