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
        Q_OBJECT

      public:
        UpAndDownBox( QQuickItem* parent )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
            setSpacing( 0 );

            auto* const topButton = new RoundButton( QskAspect::Top, this );
            connect( topButton, &QskPushButton::clicked, this, &UpAndDownBox::increase );

            auto* const bottomButton = new RoundButton( QskAspect::Bottom, this );
            connect( bottomButton, &QskPushButton::clicked, this, &UpAndDownBox::decrease );
        }

      Q_SIGNALS:
        void increase();
        void decrease();
    };
}

BoxWithButtons::BoxWithButtons( const QString& title, const QString &prefix,
                                const int initialValue, const QString &suffix,
                                bool isBright, QQuickItem* parent )
    : Box( QString(), parent )
    , m_prefix( prefix )
    , m_suffix( suffix )
{
    setSubcontrolProxy( QskBox::Panel, Panel );

    setSizePolicy( Qt::Vertical, QskSizePolicy::Maximum );

    auto* layout = new QskLinearBox( Qt::Horizontal, this );
    layout->setSpacing( 20 );

    auto iconLabel = new RoundedIcon( isBright, layout );
    iconLabel->setGraphicSource( title );
    iconLabel->setGraphicStrutSize( { 35.17, 35.17 } );
    iconLabel->setFixedSize( 68, 68 );

    auto titleAndValue = new QskLinearBox( Qt::Vertical, layout );
    titleAndValue->setPanel( true );
    titleAndValue->setSubcontrolProxy( QskBox::Panel, ValuePanel );

    auto* titleLabel = new QskTextLabel( title, titleAndValue );
    titleLabel->setFontRole( Skin::TitleFont );

    m_valueLabel = new QskTextLabel( titleAndValue );
    m_valueLabel->setSubcontrolProxy( QskTextLabel::Text, ValueText );
    setValue( initialValue );

    layout->addStretch( 1 );

    auto* const upAndDownBox = new UpAndDownBox( layout );

    connect( upAndDownBox, &UpAndDownBox::increase, this, [this]()
    {
        setValue( m_value + 1 );
    } );

    connect( upAndDownBox, &UpAndDownBox::decrease, this, [this]()
    {
        setValue( m_value - 1 );
    } );
}

void BoxWithButtons::setValue( const int value )
{
    m_value = qBound( 0, value, 100 );
    const QString text = m_prefix + QString::number( m_value ) + m_suffix;
    m_valueLabel->setText( text );
}

#include "BoxWithButtons.moc"
