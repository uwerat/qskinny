/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBox.h"
#include "QskAspect.h"

#include "QskTextOptions.h"

QSK_SUBCONTROL( QskCheckBox, Panel )
QSK_SUBCONTROL( QskCheckBox, IndicatorBox )
QSK_SUBCONTROL( QskCheckBox, IndicatorTic )
QSK_SUBCONTROL( QskCheckBox, Text )

class QskCheckBox::PrivateData {
  public:
    QString text;
    QskTextOptions textOptions;
};

QskCheckBox::QskCheckBox( QQuickItem* parent )
    : Inherited( parent ), m_data( new PrivateData )
{
    setAcceptHoverEvents( true );
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskCheckBox::~QskCheckBox()
{
}

bool QskCheckBox::isCheckable() const
{
    return true;
}

QString QskCheckBox::text() const {
    return m_data->text;
}

QskTextOptions QskCheckBox::textOptions() const {
    return m_data->textOptions;
}

void QskCheckBox::setText( const QString& text ) {
    if( m_data->text == text )
    {
        return;
    }

    m_data->text = text;

    Q_EMIT textChanged( text );
}

void QskCheckBox::setTextOptions( const QskTextOptions& textOptions) {
    if( m_data->textOptions == textOptions )
    {
        return;
    }

    m_data->textOptions = textOptions;

    Q_EMIT textOptionsChanged( textOptions );
}

#include "moc_QskCheckBox.cpp"
