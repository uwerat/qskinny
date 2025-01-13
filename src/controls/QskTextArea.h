/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_AREA_H
#define QSK_TEXT_AREA_H

#include "QskTextEdit.h"

class QSK_EXPORT QskTextArea : public QskTextEdit
{
    Q_OBJECT

    Q_PROPERTY( QString placeholderText READ placeholderText
        WRITE setPlaceholderText NOTIFY placeholderTextChanged )

    using Inherited = QskTextEdit;

  public:
    QSK_SUBCONTROLS( Panel, Placeholder )

    QskTextArea( QQuickItem* parent = nullptr );
    QskTextArea( const QString& text, QQuickItem* parent = nullptr );

    ~QskTextArea() override;

    void setPlaceholderText( const QString& );
    QString placeholderText() const;

  Q_SIGNALS:
    void placeholderTextChanged( const QString& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
