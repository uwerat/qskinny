/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskColorPicker.h"
#include "QskColorPickerSkinlet.h"
#include "QskEvent.h"
#include "QskFunctions.h"

QSK_SUBCONTROL( QskColorPicker, Panel )
QSK_SUBCONTROL( QskColorPicker, ColorPane )
QSK_SUBCONTROL( QskColorPicker, Selector )

using A = QskAspect;

namespace
{
    const auto HPos = QskColorPicker::Selector | A::Horizontal;
    const auto VPos = QskColorPicker::Selector | A::Vertical;

    void createImage( QImage* image, const QSizeF& size, qreal v )
    {
        if( image->size() != size )
        {
            *image = QImage( size.width(), size.height(), QImage::Format_RGB32 );
        }

        QColor color;
        float h, s;

        for( int x = 0; x < image->width(); x++ )
        {
            h = static_cast< float >( x ) / image->width();

            for( int y = 0; y < image->height(); y++ )
            {
                s = 1.0 - static_cast< float >( y ) / image->height();
                color.setHsvF( h, s, v );
                image->setPixel( x, y, color.rgb() );
            }
        }
    }
}

class QskColorPicker::PrivateData
{
  public:
    qreal value = 255;
    bool isPressed = false;
    QImage image;
};

QskColorPicker::QskColorPicker( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setAcceptedMouseButtons( Qt::LeftButton );
    setBoundaries( 0, 255 );
    setPolishOnResize( true );
    createImage();
    setPositionHint( HPos, -1 );
    setPositionHint( VPos, -1 );
}

QskColorPicker::~QskColorPicker() = default;

QColor QskColorPicker::selectedColor() const
{
    if( image().isNull() )
    {
        return {};
    }

    return m_data->image.pixelColor( position().toPoint() );
}

qreal QskColorPicker::value() const
{
    return m_data->value;
}

qreal QskColorPicker::valueAsRatio() const
{
    return valueAsRatio( m_data->value );
}

QImage QskColorPicker::image() const
{
    return m_data->image;
}

void QskColorPicker::setValue( qreal v )
{
    if( !qskFuzzyCompare( m_data->value, v ) )
    {
        m_data->value = v;
        createImage();
        update();
        Q_EMIT valueChanged( m_data->value );
        Q_EMIT selectedColorChanged();
    }
}

void QskColorPicker::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

void QskColorPicker::updateLayout()
{
    createImage();
    updatePosition( position() );
}

void QskColorPicker::mousePressEvent( QMouseEvent* event )
{
    m_data->isPressed = true;
    updatePosition( qskMousePosition( event ) );
}

void QskColorPicker::mouseMoveEvent( QMouseEvent* event )
{
    if( !m_data->isPressed )
    {
        return;
    }

    updatePosition( qskMousePosition( event ) );
}

void QskColorPicker::mouseReleaseEvent( QMouseEvent* )
{
    m_data->isPressed = false;
}

void QskColorPicker::updatePosition( const QPointF& point )
{
    const auto rect = subControlRect( ColorPane );

    if( rect.isEmpty() )
    {
        return;
    }

    auto p = point;
    p.rx() = qBound( rect.x(), p.x(), rect.right() - 1.0 );
    p.ry() = qBound( rect.y(), p.y(), rect.bottom() - 1.0 );

    const auto oldX = positionHint( HPos );
    const auto oldY = positionHint( VPos );

    if( !qskFuzzyCompare( p.x(), oldX ) || !qskFuzzyCompare( p.y(), oldY ) )
    {
        setPositionHint( HPos, p.x() );
        setPositionHint( VPos, p.y() );

        update();
        Q_EMIT positionChanged();
        Q_EMIT selectedColorChanged();
    }
}

QPointF QskColorPicker::position() const
{
    const auto r = subControlRect( ColorPane );

    if( !r.size().isValid() )
    {
        return {};
    }

    const auto x = positionHint( HPos );
    const auto y = positionHint( VPos );
    return { x, y };
}

void QskColorPicker::createImage()
{
    const auto r = subControlRect( ColorPane );
    ::createImage( &m_data->image, r.size(), valueAsRatio() );
}

#include "moc_QskColorPicker.cpp"
