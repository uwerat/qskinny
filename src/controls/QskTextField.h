/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_FIELD_H
#define QSK_TEXT_FIELD_H

#include "QskTextInput.h"
#include "QskGraphic.h"

class QSK_EXPORT QskTextField : public QskTextInput
{
    Q_OBJECT

    Q_PROPERTY( QString headerText READ headerText
        WRITE setHeaderText NOTIFY headerTextChanged )

    Q_PROPERTY( QString footerText READ footerText
        WRITE setFooterText NOTIFY footerTextChanged )

    Q_PROPERTY( QString placeholderText READ placeholderText
        WRITE setPlaceholderText NOTIFY placeholderTextChanged )

    Q_PROPERTY( Style style READ style
        WRITE setStyle NOTIFY styleChanged )

    using Inherited = QskTextInput;

  public:
    QSK_SUBCONTROLS( Panel, Header, Footer, Placeholder,
        Icon, Button, ButtonPanel, CharacterCount )

    enum Style : quint8
    {
        PlainStyle,

        OutlinedStyle,
        FilledStyle
    };
    Q_ENUM( Style )

    QskTextField( QQuickItem* parent = nullptr );
    QskTextField( const QString& text, QQuickItem* parent = nullptr );

    ~QskTextField() override;

    void setStyle( Style );
    Style style() const;

    void setHeaderText( const QString& );
    QString headerText() const;

    void setFooterText( const QString& );
    QString footerText() const;

    QskGraphic icon() const;
    void setIcon( const QskGraphic& );

    void setPlaceholderText( const QString& );
    QString placeholderText() const;

    QskAspect::Variation effectiveVariation() const override;

  Q_SIGNALS:
    void headerTextChanged( const QString& );
    void footerTextChanged( const QString& );
    void placeholderTextChanged( const QString& );
    void styleChanged( Style );

  protected:
    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverMoveEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
