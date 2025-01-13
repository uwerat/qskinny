/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextArea.h"

QSK_SUBCONTROL( QskTextArea, Panel )
QSK_SUBCONTROL( QskTextArea, PlaceholderText )

class QskTextArea::PrivateData
{
  public:
    QString placeholderText;
};

QskTextArea::QskTextArea( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
}

QskTextArea::QskTextArea( const QString& text, QQuickItem* parent )
    : QskTextArea( parent )
{
    setText( text );
}

QskTextArea::~QskTextArea()
{
}

void QskTextArea::setPlaceholderText( const QString& text )
{
    if ( m_data->placeholderText != text )
    {
        m_data->placeholderText = text;
        Q_EMIT placeholderTextChanged( text );
    }
}

QString QskTextArea::placeholderText() const
{
    return m_data->placeholderText;
}

#include "moc_QskTextArea.cpp"
