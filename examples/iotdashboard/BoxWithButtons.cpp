/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "BoxWithButtons.h"
#include "RoundButton.h"
#include "RoundedIcon.h"
#include "Skin.h"

#include <QskTextLabel.h>
#include <QskLinearBox.h>

QSK_SUBCONTROL( BoxWithButtons, ValueText )
QSK_SUBCONTROL( BoxWithButtons, ValuePanel )
QSK_SUBCONTROL( BoxWithButtons, Panel )

namespace
{
    class UpAndDownBox : public QskLinearBox
    {
      public:
        UpAndDownBox( QQuickItem* parent )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
            setSpacing( 0 );

            new RoundButton( QskAspect::Top, this );
            new RoundButton( QskAspect::Bottom, this );
        }
    };
}

BoxWithButtons::BoxWithButtons( const QString& title, const QString& value,
        bool isBright, QQuickItem* parent )
    : Box( QString(), parent )
{
    setPanel( true );
    setSubcontrolProxy( QskBox::Panel, Panel );

    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    QString iconFile = title.toLower();
    iconFile = iconFile.replace( ' ', '-' );
    new RoundedIcon( iconFile, isBright, false, layout );

    auto titleAndValue = new QskLinearBox( Qt::Vertical, layout );
    titleAndValue->setPanel( true );
    titleAndValue->setSubcontrolProxy( QskBox::Panel, ValuePanel );

    auto* titleLabel = new QskTextLabel( title, titleAndValue );
    titleLabel->setFontRole( Skin::TitleFont );

    auto valueLabel = new QskTextLabel( value, titleAndValue );
    valueLabel->setSubcontrolProxy( QskTextLabel::Text, ValueText );

    new UpAndDownBox( layout );
}
