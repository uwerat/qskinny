/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_H
#define QSK_SUB_WINDOW_H 1

#include "QskPopup.h"

class QskGraphic;
class QskTextOptions;
class QUrl;

class QSK_EXPORT QskSubWindow : public QskPopup
{
    Q_OBJECT

    Q_PROPERTY( bool decorated READ isDecorated
        WRITE setDecorated NOTIFY decoratedChanged )

    Q_PROPERTY( QString windowTitle READ windowTitle
        WRITE setWindowTitle NOTIFY windowTitleChanged )

    Q_PROPERTY( QskTextOptions windowTitleTextOptions READ windowTitleTextOptions
        WRITE setWindowTitleTextOptions NOTIFY windowTitleTextOptionsChanged )

    Q_PROPERTY( QUrl windowIconSource READ windowIconSource
        WRITE setWindowIconSource NOTIFY windowIconSourceChanged )

    Q_PROPERTY( QskGraphic windowIcon READ windowIcon
        WRITE setWindowIcon NOTIFY windowIconChanged FINAL )

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

    QSK_SUBCONTROLS( Panel, TitleBar, TitleBarSymbol, TitleBarText )

    QskSubWindow( QQuickItem* parent = nullptr );
    ~QskSubWindow() override;

    void setDecorated( bool );
    bool isDecorated() const;

    void setWindowTitleTextOptions( const QskTextOptions& );
    QskTextOptions windowTitleTextOptions() const;

    void setWindowTitle( const QString& );
    QString windowTitle() const;

    void setWindowIconSource( const QString& );
    void setWindowIconSource( const QUrl& );
    QUrl windowIconSource() const;

    void setWindowIcon( const QskGraphic& );
    QskGraphic windowIcon() const;

    bool hasWindowIcon() const;

    void setWindowButtons( WindowButtons );
    WindowButtons windowButtons() const;

    void setWindowButton( WindowButton, bool on = true );
    bool testWindowButton( WindowButton ) const;

    QRectF titleBarRect() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;

  Q_SIGNALS:
    void decoratedChanged();
    void windowTitleChanged();
    void windowTitleTextOptionsChanged();
    void windowIconChanged();
    void windowIconSourceChanged();
    void windowButtonsChanged();

  protected:
    bool event( QEvent* ) override;

    void updateLayout() override;
    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    void itemChange( QQuickItem::ItemChange,
        const QQuickItem::ItemChangeData& ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
