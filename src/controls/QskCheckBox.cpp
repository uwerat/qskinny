/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBox.h"

QSK_SUBCONTROL( QskCheckBox, Panel )
QSK_SUBCONTROL( QskCheckBox, Box )
QSK_SUBCONTROL( QskCheckBox, Indicator )
QSK_SUBCONTROL( QskCheckBox, Text )

class QskCheckBox::PrivateData
{
  public:
    PrivateData( const QString& text )
        : text( text )
    {
    }

    QString text;
};

QskCheckBox::QskCheckBox( QQuickItem* parent )
    : QskCheckBox( QString(), parent )
{
}

QskCheckBox::QskCheckBox( const QString& text, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( text ) )
{
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskCheckBox::~QskCheckBox()
{
}

bool QskCheckBox::isCheckable() const
{
    return true;
}

void QskCheckBox::setText( const QString& text )
{
    if ( text != m_data->text )
    {
        m_data->text = text;

        resetImplicitSize();
        update();

        Q_EMIT textChanged();
    }
}

QString QskCheckBox::text() const
{
    return m_data->text;
}

void QskCheckBox::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::LayoutDirectionChange )
    {
        if ( !m_data->text.isEmpty() )
            update();
    }

    Inherited::changeEvent( event );
}

#include "moc_QskCheckBox.cpp"
