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
    class Rectangle : public QskControl
    {
      public:
        Rectangle( const QByteArray& colorName )
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

    class Grid : public QskGridBox
    {
      public:
        Grid( QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
            setBackgroundColor( Qt::white );
            setMargins( 10 );
        }
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

    setContent( QUrl(), nullptr, m_grid );
}

GridSkinny::~GridSkinny()
{
}

void GridSkinny::insert( const QByteArray& colorName,
    int row, int column, int rowSpan, int columnSpan )
{
    m_grid->addItem( new Rectangle( colorName ),
        row, column, rowSpan, columnSpan );
}

void GridSkinny::setSpacing( Qt::Orientations orientations, int spacing )
{
    m_grid->setSpacing( orientations, spacing );
}

void GridSkinny::setRowSizeHint( int row, Qt::SizeHint which, int height )
{
    m_grid->setRowSizeHint( row, which, height );
}

void GridSkinny::setColumnSizeHint( int column, Qt::SizeHint which, int width )
{
    m_grid->setColumnSizeHint( column, which, width );
}

void GridSkinny::setSizeHint( int index, Qt::Orientation orientation,
    Qt::SizeHint which, int hint )
{
    if ( auto control = qobject_cast< QskControl* >( m_grid->itemAtIndex( index ) ) )
    {
        auto size = control->explicitSizeHint( which );

        if ( orientation == Qt::Horizontal )
            size.setWidth( hint );
        else
            size.setHeight( hint );

        control->setExplicitSizeHint( which, size );
    }
}

void GridSkinny::setSizePolicy(
    int index, Qt::Orientation orientation, int policy )
{
    if ( auto control = qobject_cast< QskControl* >( m_grid->itemAtIndex( index ) ) )
    {
        control->setSizePolicy( orientation,
            static_cast< QskSizePolicy::Policy >( policy ) );
    }
}

void GridSkinny::setAlignment( int index, Qt::Alignment alignment )
{
    if ( auto item = m_grid->itemAtIndex( index ) )
        m_grid->setAlignment( item, alignment );
}

void GridSkinny::setRetainSizeWhenHidden( int index, bool on )
{
    if ( auto item = m_grid->itemAtIndex( index ) )
        m_grid->setRetainSizeWhenHidden( item, on );

}
