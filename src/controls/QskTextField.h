/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_FIELD_H
#define QSK_TEXT_FIELD_H

#include "QskTextInput.h"

class QSK_EXPORT QskTextField : public QskTextInput
{
    Q_OBJECT

    Q_PROPERTY( QString placeholderText READ placeholderText
        WRITE setPlaceholderText NOTIFY placeholderTextChanged )

    using Inherited = QskTextInput;

  public:
    QSK_SUBCONTROLS( Panel, PlaceholderText )

    QskTextField( QQuickItem* parent = nullptr );
    QskTextField( const QString& text, QQuickItem* parent = nullptr );

    ~QskTextField() override;

    void setPlaceholderText( const QString& );
    QString placeholderText() const;

  Q_SIGNALS:
    void placeholderTextChanged( const QString& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
