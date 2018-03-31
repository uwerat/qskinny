/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskResizable.h"

// QGridLayoutEngine internally uses FLT_MAX
static constexpr qreal c_max = std::numeric_limits< float >::max();

QskResizable::QskResizable():
    m_sizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred ),
    m_sizeHints{ { 0, 0 }, { -1, -1 }, { c_max, c_max } }
{
}

QskResizable::~QskResizable()
{
}

void QskResizable::initSizePolicy(
    QskSizePolicy::Policy horizontalPolicy,
    QskSizePolicy::Policy verticalPolicy )
{
    /*
       In constructors of derived classes you don't need
       to propagate changes by layoutConstraintChanged.
       Sometimes it is even worse as the parent might not be
       even prepared to handle the LayouRequest event.
     */
    
    m_sizePolicy.setHorizontalPolicy( horizontalPolicy );
    m_sizePolicy.setVerticalPolicy( verticalPolicy );
}

void QskResizable::setSizePolicy( const QskSizePolicy& policy )
{
    if ( policy != m_sizePolicy )
    {
        m_sizePolicy = policy;
        layoutConstraintChanged();
    }
}

void QskResizable::setSizePolicy(
    QskSizePolicy::Policy horizontalPolicy,
    QskSizePolicy::Policy verticalPolicy )
{
    setSizePolicy( QskSizePolicy( horizontalPolicy, verticalPolicy ) );
}

void QskResizable::setSizePolicy(
    Qt::Orientation orientation, QskSizePolicy::Policy policy )
{
    if ( m_sizePolicy.policy( orientation ) != policy )
    {
        m_sizePolicy.setPolicy( orientation, policy );
        layoutConstraintChanged();
    }
}

const QskSizePolicy& QskResizable::sizePolicy() const
{
    return m_sizePolicy;
}

QskSizePolicy::Policy QskResizable::sizePolicy( Qt::Orientation orientation ) const
{
    return m_sizePolicy.policy( orientation );
}

QSizeF QskResizable::preferredSize() const
{
    return sizeHint( Qt::PreferredSize );
}

void QskResizable::setPreferredSize( const QSizeF& size )
{
    setSizeHint( Qt::PreferredSize, size );
}

void QskResizable::setPreferredSize( qreal width, qreal height )
{
    setPreferredSize( QSizeF( width, height ) );
}

void QskResizable::setPreferredWidth( qreal width )
{
    setPreferredSize( QSizeF( width, preferredSize().height() ) );
}

void QskResizable::setPreferredHeight( qreal height )
{
    setPreferredSize( QSizeF( preferredSize().width(), height ) );
}

QSizeF QskResizable::minimumSize() const
{
    return sizeHint( Qt::MinimumSize );
}

void QskResizable::setMinimumSize( const QSizeF& size )
{
    setSizeHint( Qt::MinimumSize, size );
}

void QskResizable::setMinimumSize( qreal width, qreal height )
{
    setMinimumSize( QSizeF( width, height ) );
}

void QskResizable::setMinimumWidth( qreal width )
{
    setMinimumSize( QSizeF( width, minimumSize().height() ) );
}

void QskResizable::setMinimumHeight( qreal height )
{
    setMinimumSize( QSizeF( minimumSize().width(), height ) );
}

QSizeF QskResizable::maximumSize() const
{
    return sizeHint( Qt::MaximumSize );
}

void QskResizable::setMaximumSize( const QSizeF& size )
{
    setSizeHint( Qt::MaximumSize, size );
}

void QskResizable::setMaximumSize( qreal width, qreal height )
{
    setMaximumSize( QSizeF( width, height ) );
}

void QskResizable::setMaximumWidth( qreal width )
{
    setMaximumSize( QSizeF( width, maximumSize().height() ) );
}

void QskResizable::setMaximumHeight( qreal height )
{
    setMaximumSize( QSizeF( maximumSize().width(), height ) );
}

void QskResizable::setFixedSize( const QSizeF& size )
{
    const QSizeF newSize = size.expandedTo( QSizeF( 0, 0 ) );

    const QskSizePolicy policy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    if ( policy != m_sizePolicy || m_sizeHints[Qt::PreferredSize] != newSize )
    {
        m_sizePolicy = policy;
        m_sizeHints[Qt::PreferredSize] = newSize;

        layoutConstraintChanged();
    }
}

void QskResizable::setFixedSize( qreal width, qreal height )
{
    setFixedSize( QSizeF( width, height ) );
}

void QskResizable::setFixedWidth( qreal width )
{
    if ( width < 0 )
        width = 0;

    if ( m_sizePolicy.horizontalPolicy() != QskSizePolicy::Fixed
         || m_sizeHints[Qt::PreferredSize].width() != width )
    {
        m_sizePolicy.setHorizontalPolicy( QskSizePolicy::Fixed );
        m_sizeHints[Qt::PreferredSize].setWidth( width );

        layoutConstraintChanged();
    }
}

void QskResizable::setFixedHeight( qreal height )
{
    if ( height < 0 )
        height = 0;

    if ( m_sizePolicy.verticalPolicy() != QskSizePolicy::Fixed
         || m_sizeHints[Qt::PreferredSize].height() != height )
    {
        m_sizePolicy.setVerticalPolicy( QskSizePolicy::Fixed );
        m_sizeHints[Qt::PreferredSize].setHeight( height );

        layoutConstraintChanged();
    }
}

void QskResizable::resetSizeHint( Qt::SizeHint whichHint )
{
    QSizeF hint;

    switch( whichHint )
    {
        case Qt::MinimumSize:
        {
            hint = QSizeF( 0, 0 );
            break;
        }
        case Qt::MaximumSize:
        {
            hint = QSizeF( c_max, c_max );
            break;
        }
        case Qt::PreferredSize:
        {
            hint = QSizeF( -1, -1 );
            break;
        }
        default:
            return;
    }

    if ( hint != m_sizeHints[whichHint] )
    {
        m_sizeHints[whichHint] = hint;
        layoutConstraintChanged();
    }
}

void QskResizable::setSizeHint( Qt::SizeHint whichHint, const QSizeF& size )
{
    if ( whichHint >= Qt::MinimumSize && whichHint <= Qt::MaximumSize )
    {
        const QSizeF newSize( ( size.width() < 0 ) ? -1.0 : size.width(),
            ( size.width() < 0 ) ? -1.0 : size.width() );

        if ( m_sizeHints[whichHint] != size )
        {
            m_sizeHints[whichHint] = size;
            layoutConstraintChanged();
        }
    }
}

void QskResizable::setSizeHint( Qt::SizeHint whichHint, qreal width, qreal height )
{
    setSizeHint( whichHint, QSizeF( width, height ) );
}

QSizeF QskResizable::sizeHint( Qt::SizeHint whichHint ) const
{
    if ( whichHint >= Qt::MinimumSize && whichHint <= Qt::MaximumSize )
        return m_sizeHints[whichHint];

    // Qt::MinimumDescent ???
    return QSizeF( -1, -1 );
}

qreal QskResizable::heightForWidth( qreal width ) const
{
    Q_UNUSED( width )
    return -1;
}

qreal QskResizable::widthForHeight( qreal height ) const
{
    Q_UNUSED( height )
    return -1;
}

void QskResizable::layoutConstraintChanged()
{
}
