/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "GridSkinny.h"
#include <QskControl.h>
#include <QskGridBox.h>
#include <QskQuick.h>

namespace
{
    class ColoredRectangle : public QskControl
    {
      public:
        ColoredRectangle( const QByteArray& colorName )
            : m_colorName( colorName )
        {
            setObjectName( colorName );

            initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );
            setPreferredSize( 50, 50 );

            setBackgroundColor( colorName.constData() );
        }

      protected:
        void geometryChangeEvent( QskGeometryChangeEvent* event ) override
        {
            QskControl::geometryChangeEvent( event );
            //qDebug() << colorName() << size();
        }

      private:
        QByteArray m_colorName;
    };
}

GridSkinny::GridSkinny( QWidget* parent )
    : QQuickWidget( parent )
{
    setContentsMargins( QMargins() );
    setResizeMode( QQuickWidget::SizeRootObjectToView );

    m_grid = new QskGridBox();
    m_grid->setBackgroundColor( Qt::white );
    m_grid->setMargins( 10 );
    m_grid->setSpacing( 5 );

    setContent( QUrl(), nullptr, m_grid );
}

GridSkinny::~GridSkinny()
{
}

void GridSkinny::insert( const QByteArray& colorName,
    int row, int column, int rowSpan, int columnSpan )
{
    m_grid->addItem( new ColoredRectangle( colorName ),
        row, column, rowSpan, columnSpan );
}

void GridSkinny::setSpacing( Qt::Orientations orientations, int spacing )
{
    m_grid->setSpacing( orientations, spacing );
}

void GridSkinny::setSizeHint(
    int pos, Qt::Orientation orientation, Qt::SizeHint which, int hint )
{
    if ( orientation == Qt::Vertical )
        m_grid->setRowHeightHint( pos, which, hint );
    else
        m_grid->setColumnWidthHint( pos, which, hint );
}

void GridSkinny::setStretchFactor(
    int pos, Qt::Orientation orientation, int stretch )
{
    if ( orientation == Qt::Vertical )
        m_grid->setRowStretchFactor( pos, stretch );
    else
        m_grid->setColumnStretchFactor( pos, stretch );
}

void GridSkinny::setSizeHintAt( int index, Qt::Orientation orientation,
    Qt::SizeHint which, int hint )
{
    if ( auto control = qskControlCast( m_grid->itemAtIndex( index ) ) )
    {
        auto size = control->explicitSizeHint( which );

        if ( orientation == Qt::Horizontal )
            size.setWidth( hint );
        else
            size.setHeight( hint );

        control->setExplicitSizeHint( which, size );
    }
}

void GridSkinny::setSizePolicyAt(
    int index, Qt::Orientation orientation, int policy )
{
    if ( auto control = qskControlCast( m_grid->itemAtIndex( index ) ) )
    {
        control->setSizePolicy( orientation,
            static_cast< QskSizePolicy::Policy >( policy ) );
    }
}

void GridSkinny::setAlignmentAt( int index, Qt::Alignment alignment )
{
    if ( auto control = qskControlCast( m_grid->itemAtIndex( index ) ) )
        control->setLayoutAlignmentHint( alignment );
}

void GridSkinny::setRetainSizeWhenHiddenAt( int index, bool on )
{
    if ( auto control = qskControlCast( m_grid->itemAtIndex( index ) ) )
        control->setLayoutHint( QskControl::RetainSizeWhenHidden, on );
}

void GridSkinny::setVisibleAt( int index, bool on )
{
    if ( auto item = m_grid->itemAtIndex( index ) )
        item->setVisible( on );
}

QSize GridSkinny::preferredSize() const
{
    return m_grid->preferredSize().toSize();
}
