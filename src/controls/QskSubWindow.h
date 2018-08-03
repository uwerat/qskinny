/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_H
#define QSK_SUB_WINDOW_H 1

#include "QskPopup.h"

class QSK_EXPORT QskSubWindow : public QskPopup
{
    Q_OBJECT

    Q_PROPERTY( bool decorated READ isDecorated
        WRITE setDecorated NOTIFY decoratedChanged )

    Q_PROPERTY( QString title READ title
        WRITE setTitle NOTIFY titleChanged )

    Q_PROPERTY( WindowButtons windowButtons READ windowButtons
        WRITE setWindowButtons NOTIFY windowButtonsChanged )

    using Inherited = QskPopup;

  public:
    enum WindowButton
    {
        MinimizeButton = 0x1,
        MaximizeButton = 0x2,
        CloseButton = 0x4
    };

    Q_ENUM( WindowButton )
    Q_DECLARE_FLAGS( WindowButtons, WindowButton )

    QSK_SUBCONTROLS( Panel, TitleBar )

    QskSubWindow( QQuickItem* parent = nullptr );
    ~QskSubWindow() override;

    Q_INVOKABLE void setTitle( const QString& );
    Q_INVOKABLE QString title() const;

    Q_INVOKABLE void setDecorated( bool );
    Q_INVOKABLE bool isDecorated() const;

    Q_INVOKABLE void setWindowButtons( WindowButtons );
    Q_INVOKABLE WindowButtons windowButtons() const;

    Q_INVOKABLE void setWindowButton( WindowButton, bool on = true );
    Q_INVOKABLE bool testWindowButton( WindowButton ) const;

    QRectF titleBarRect() const;

    QSizeF contentsSizeHint() const override;
    QRectF layoutRect() const override;

  Q_SIGNALS:
    void titleChanged();
    void decoratedChanged();
    void windowButtonsChanged();

  protected:
    bool event( QEvent* ) override;

    void itemChange( QQuickItem::ItemChange,
        const QQuickItem::ItemChangeData& ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
