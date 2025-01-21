/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_H
#define QSK_SUB_WINDOW_H

#include "QskPopup.h"

class QskGraphic;
class QskTextOptions;
class QUrl;

class QSK_EXPORT QskSubWindow : public QskPopup
{
    Q_OBJECT

    Q_PROPERTY( Decorations decorations READ decorations
        WRITE setDecorations RESET resetDecorations NOTIFY decorationsChanged )

    Q_PROPERTY( QString windowTitle READ windowTitle
        WRITE setWindowTitle NOTIFY windowTitleChanged )

    Q_PROPERTY( QskTextOptions windowTitleTextOptions READ windowTitleTextOptions
        WRITE setWindowTitleTextOptions NOTIFY windowTitleTextOptionsChanged )

    Q_PROPERTY( QUrl windowIconSource READ windowIconSource
        WRITE setWindowIconSource NOTIFY windowIconSourceChanged )

    Q_PROPERTY( QskGraphic windowIcon READ windowIcon
        WRITE setWindowIcon NOTIFY windowIconChanged FINAL )

    using Inherited = QskPopup;

  public:
    enum Decoration : quint16
    {
        NoDecoration   = 0,

        TitleBar       = 1 << 0,

        Title          = 1 << 1,
        Symbol         = 1 << 2

#if 0
        MinimizeButton = 1 << 3,
        MaximizeButton = 1 << 4,
        CloseButton    = 1 << 5
#endif
    };

    Q_ENUM( Decoration )
    Q_DECLARE_FLAGS( Decorations, Decoration )

    QSK_SUBCONTROLS( Panel, TitleBarPanel, TitleBarSymbol, TitleBarText )

    QskSubWindow( QQuickItem* parent = nullptr );
    ~QskSubWindow() override;

    void setDecorations( Decorations );
    void resetDecorations();
    Decorations decorations() const;

    void setDecoration( Decoration, bool on = true );
    bool hasDecoration( Decoration ) const;

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

    QRectF titleBarRect() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;
    QskAspect fadingAspect() const override;

  Q_SIGNALS:
    void decorationsChanged( Decorations );
    void windowTitleChanged();
    void windowTitleTextOptionsChanged();
    void windowIconChanged();
    void windowIconSourceChanged();

  protected:
    bool event( QEvent* ) override;

    void updateLayout() override;
    void updateResources() override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void geometryChange( const QRectF&, const QRectF& ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskSubWindow::Decorations )
Q_DECLARE_METATYPE( QskSubWindow::Decorations )

#endif
