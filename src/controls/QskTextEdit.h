/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_EDIT_H
#define QSK_TEXT_EDIT_H

#include "QskAbstractTextInput.h"
#include "QskTextOptions.h"

class QSK_EXPORT QskTextEdit : public QskAbstractTextInput
{
    Q_OBJECT

    Q_PROPERTY( int lineCount READ lineCount NOTIFY lineCountChanged )

    Q_PROPERTY( QskTextOptions::TextFormat textFormat READ textFormat
        WRITE setTextFormat NOTIFY textFormatChanged )

    Q_PROPERTY( qreal tabStopDistance READ tabStopDistance
        WRITE setTabStopDistance NOTIFY tabStopDistanceChanged )

    Q_PROPERTY( QUrl baseUrl READ baseUrl WRITE setBaseUrl
        RESET resetBaseUrl NOTIFY baseUrlChanged )

    using Inherited = QskAbstractTextInput;

  public:
    QSK_SUBCONTROLS( Text, TextPanel )

    QskTextEdit( QQuickItem* parent = nullptr );
    ~QskTextEdit() override;

    void setTextFormat( QskTextOptions::TextFormat );
    QskTextOptions::TextFormat textFormat() const;

    int lineCount() const;

    int tabStopDistance() const;
    void setTabStopDistance( qreal );

    QUrl baseUrl() const;
    void setBaseUrl( const QUrl& );
    void resetBaseUrl();

  Q_SIGNALS:
    void lineCountChanged( int );
    void baseUrlChanged( QUrl );

    void textFormatChanged( QskTextOptions::TextFormat );
    void tabStopDistanceChanged( qreal );

    void linkHovered( const QString& );
    void linkActivated( const QString& );

  protected:
    QskAspect::Subcontrol substitutedSubcontrol(
        QskAspect::Subcontrol ) const override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
