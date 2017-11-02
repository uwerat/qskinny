/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_WINDOW_H
#define QSK_WINDOW_H 1

#include "QskGlobal.h"
#include <QQuickWindow>

class QskWindowPrivate;
class QskObjectAttributes;

class QSK_EXPORT QskWindow : public QQuickWindow
{
    Q_OBJECT

    Q_PROPERTY( bool deleteOnClose READ deleteOnClose
        WRITE setDeleteOnClose NOTIFY deleteOnCloseChanged FINAL )

    Q_PROPERTY( bool autoLayoutChildren READ autoLayoutChildren
        WRITE setAutoLayoutChildren NOTIFY autoLayoutChildrenChanged FINAL )

    Q_PROPERTY( QLocale locale READ locale
        WRITE setLocale RESET resetLocale NOTIFY localeChanged FINAL )

    Q_PROPERTY( FramebufferMode framebufferMode READ framebufferMode
        WRITE setFramebufferMode NOTIFY framebufferModeChanged FINAL )

    using Inherited = QQuickWindow;

public:
    enum FramebufferMode
    {
        DefaultFramebufferMode = 0,
        OffscreenFramebufferMode = 1
    };

    Q_ENUM( FramebufferMode )

    QskWindow( QWindow* parent = nullptr );
    virtual ~QskWindow();

    bool deleteOnClose() const;
    void setDeleteOnClose( bool );

    void setAutoLayoutChildren( bool );
    bool autoLayoutChildren() const;

    Q_INVOKABLE void addItem( QQuickItem* );

    QLocale locale() const;
    void resetLocale();

    Q_INVOKABLE void setPreferredSize( const QSize& );
    Q_INVOKABLE QSize preferredSize() const;

    Q_INVOKABLE QSize effectivePreferredSize() const;

    Q_INVOKABLE void setFixedSize( const QSize& );

    void setFramebufferMode( FramebufferMode );
    FramebufferMode framebufferMode() const;

    void polishItems();

    void setCustomRenderMode( const char* mode );
    const char* customRenderMode() const;

Q_SIGNALS:
    void localeChanged( const QLocale& );
    void framebufferModeChanged( FramebufferMode );
    void autoLayoutChildrenChanged();
    void deleteOnCloseChanged();

public Q_SLOTS:
    void setLocale( const QLocale& );
    void resizeF( const QSizeF& );

protected:
    virtual bool event( QEvent* ) override;
    virtual void resizeEvent( QResizeEvent* ) override;
    virtual void exposeEvent( QExposeEvent* ) override;

    virtual void layoutItems();

private:
    void resizeFramebuffer();
    void blitFramebuffer();
    void enforceSkin();

    Q_DECLARE_PRIVATE( QskWindow )
};

#endif
