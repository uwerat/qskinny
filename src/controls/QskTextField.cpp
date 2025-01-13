/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextField.h"

QSK_SUBCONTROL( QskTextField, Panel )
QSK_SUBCONTROL( QskTextField, Placeholder )

class QskTextField::PrivateData
{
  public:
    QString placeholderText;
};

QskTextField::QskTextField( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
}

QskTextField::QskTextField( const QString& text, QQuickItem* parent )
    : QskTextField( parent )
{
    setText( text );
}

QskTextField::~QskTextField()
{
}

void QskTextField::setPlaceholderText( const QString& text )
{
    if ( m_data->placeholderText != text )
    {
        m_data->placeholderText = text;
        Q_EMIT placeholderTextChanged( text );
    }
}

QString QskTextField::placeholderText() const
{
    return m_data->placeholderText;
}

#include "moc_QskTextField.cpp"
