/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_LABEL_H
#define QSK_TEXT_LABEL_H

#include "QskControl.h"
#include "QskTextOptions.h"

class QSK_EXPORT QskTextLabel : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged )

    Q_PROPERTY( int fontRole READ fontRole
        WRITE setFontRole NOTIFY fontRoleChanged )

    Q_PROPERTY( QColor textColor READ textColor
        WRITE setTextColor NOTIFY textColorChanged )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    Q_PROPERTY( Qt::Alignment alignment READ alignment
        WRITE setAlignment NOTIFY alignmentChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Text )

    QskTextLabel( QQuickItem* parent = nullptr );
    QskTextLabel( const QString& text, QQuickItem* parent = nullptr );

    ~QskTextLabel() override;

    QString text() const;

    void setFontRole( int role );
    int fontRole() const;

    void setTextColor( const QColor& );
    QColor textColor() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    void setTextFormat( QskTextOptions::TextFormat );
    QskTextOptions::TextFormat textFormat() const;

    void setWrapMode( QskTextOptions::WrapMode );
    QskTextOptions::WrapMode wrapMode() const;

    void setElideMode( Qt::TextElideMode );
    Qt::TextElideMode elideMode() const;

    void setAlignment( Qt::Alignment );
    Qt::Alignment alignment() const;

    QSizeF contentsSizeHint() const override;

    qreal heightForWidth( qreal width ) const override;
    qreal widthForHeight( qreal height ) const override;

    QFont font() const;

  Q_SIGNALS:
    void textChanged( const QString& );
    void textColorChanged( const QColor& );
    void textOptionsChanged( const QskTextOptions& );
    void fontRoleChanged();
    void alignmentChanged();

  public Q_SLOTS:
    void setText( const QString& );

  protected:
    void changeEvent( QEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
