/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_BUTTON_H
#define QSK_TAB_BUTTON_H

#include "QskAbstractButton.h"

class QskTextOptions;
class QskTabBar;

class QSK_EXPORT QskTabButton : public QskAbstractButton
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText NOTIFY textChanged FINAL )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    using Inherited = QskAbstractButton;

  public:
    QSK_SUBCONTROLS( Panel, Text )

    QskTabButton( QQuickItem* parent = nullptr );
    QskTabButton( const QString& text, QQuickItem* parent = nullptr );

    ~QskTabButton() override;

    void setText( const QString& text );
    QString text() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;

    QskAspect::Placement effectivePlacement() const override;

    const QskTabBar* tabBar() const;
    QskTabBar* tabBar();

  Q_SIGNALS:
    void textChanged( const QString& text );
    void textOptionsChanged();

  protected:
    void changeEvent( QEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
