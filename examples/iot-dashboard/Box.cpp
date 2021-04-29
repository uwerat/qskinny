/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Box.h"
#include "Skin.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( Box, Panel )

Box::Box( const QString& title, QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
    , m_title( title )
    , m_label( new QskTextLabel( m_title, this ) )
{
    setAutoAddChildren( true );
    setAutoLayoutChildren( true );
    setPanel( true );

    m_label->setFontRole( Skin::TitleFont );

    if( m_label->text().isEmpty() )
    {
        m_label->setVisible( false );
    }
}

QskAspect::Subcontrol Box::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return Box::Panel;
    }

    return subControl;
}

#include "moc_Box.cpp"
