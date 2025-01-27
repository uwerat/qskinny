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

class QskColorPicker::PrivateData
{
  public:
    qreal value = 255;
    bool isPressed = false;
    int colorChangedEventType;
};

QskColorPicker::QskColorPicker( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    m_data->colorChangedEventType = QEvent::registerEventType();

    setAcceptedMouseButtons( Qt::LeftButton );
    setBoundaries( 0, 255 );
}

QskColorPicker::~QskColorPicker() = default;

QColor QskColorPicker::selectedColor() const
{
    auto* skinlet = static_cast< const QskColorPickerSkinlet* >( effectiveSkinlet() );
    Q_ASSERT( skinlet );
    return skinlet->selectedColor();
}

qreal QskColorPicker::value() const
{
    return m_data->value;
}

qreal QskColorPicker::valueAsRatio() const
{
    return valueAsRatio( m_data->value );
}

int QskColorPicker::colorChangedEventType() const
{
    return m_data->colorChangedEventType;
}

void QskColorPicker::setValue( qreal v )
{
    if( !qskFuzzyCompare( m_data->value, v ) )
    {
        m_data->value = v;
        update();
        Q_EMIT valueChanged( m_data->value );
    }
}

void QskColorPicker::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

bool QskColorPicker::event( QEvent* event )
{
    if( event->type() == colorChangedEventType() )
    {
        event->setAccepted( true );
        Q_EMIT selectedColorChanged();
        return true;
    }

    return Inherited::event( event );
}

void QskColorPicker::mousePressEvent( QMouseEvent* event )
{
    m_data->isPressed = true;
    updatePosition( event );
}

void QskColorPicker::mouseMoveEvent( QMouseEvent* event )
{
    if( !m_data->isPressed )
    {
        return;
    }

    updatePosition( event );
}

void QskColorPicker::mouseReleaseEvent( QMouseEvent* )
{
    m_data->isPressed = false;
}

void QskColorPicker::updatePosition( QMouseEvent* event )
{
    auto p = qskMousePosition( event );
    const auto rect = subControlRect( ColorPane );

    p.rx() = qBound( rect.x(), p.x(), rect.right() );
    p.ry() = qBound( rect.y(), p.y(), rect.bottom() );

    const auto oldX = positionHint( Selector | QskAspect::Horizontal );
    const auto oldY = positionHint( Selector | QskAspect::Vertical );

    if( !qskFuzzyCompare( p.x(), oldX ) || !qskFuzzyCompare( p.y(), oldY ) )
    {
        setPositionHint( Selector | QskAspect::Horizontal, p.x() );
        setPositionHint( Selector | QskAspect::Vertical, p.y() );

        update();
    }
}

#include "moc_QskColorPicker.cpp"
