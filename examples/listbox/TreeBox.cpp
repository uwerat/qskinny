/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "TreeBox.h"

QSK_SUBCONTROL( TreeBox, FirstRow )
QSK_SUBCONTROL( TreeBox, SecondRow )

TreeBox::TreeBox( QQuickItem* const parent )
    : Inherited( parent )
{
    setAlternatingRowColors( true );   
    setPaddingHint( Cell, QMargins( 10, 20, 10, 20 ) );

    setAlternatingRowColors( true );
    setColor( FirstRow, Qt::white );
    setColor( SecondRow, color(Panel) );
    updateScrollableSize();
};

QskAspect::Subcontrol TreeBox::rowSubControl( int row ) const noexcept
{
    if ( alternatingRowColors() )
    {
        if ( row == selectedRow() )
        {
            return Cell;
        }
        return row % 2 == 0 ? Cell : QskAspect::NoSubcontrol;
    }
    return QskAspect::NoSubcontrol;
}

int TreeBox::rowCount() const
{
    return 100000;
}

int TreeBox::columnCount() const
{
    return 2;
}

qreal TreeBox::columnWidth( int col ) const
{    
    return col == 0 ? 100: 350;
}

qreal TreeBox::rowHeight() const
{
    const auto hint = strutSizeHint( Cell );
    const auto padding = paddingHint( Cell );

    qreal h = effectiveFontHeight( Text );
    h += padding.top() + padding.bottom();

    return qMax( h, hint.height() );
}

Q_INVOKABLE QVariant TreeBox::valueAt( int row, int col ) const
{
    if ( col == 0 )
    {
        return QString::number( row );
    }
    return QStringLiteral( "The quick brown fox jumps over the lazy dog" );
}

qreal TreeBox::rowOffset( int row ) const 
{
    return ( row % 4 ) * 10;
}
