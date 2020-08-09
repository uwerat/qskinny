/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_WINDOW_H
#define QSK_WINDOW_H 1

#include "QskGlobal.h"
#include <qquickwindow.h>

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

    using Inherited = QQuickWindow;

  public:
    enum EventAcceptance
    {
        EventProcessed = 0,
        EventPropagationStopped = 1
    };

    QskWindow( QWindow* parent = nullptr );
    QskWindow( QQuickRenderControl* renderControl, QWindow* parent = nullptr );

    ~QskWindow() override;

    using Inherited::setScreen;
    void setScreen( const QString& );

    bool deleteOnClose() const;
    void setDeleteOnClose( bool );

    void setAutoLayoutChildren( bool );
    bool autoLayoutChildren() const;

    Q_INVOKABLE void addItem( QQuickItem* );

    QLocale locale() const;
    void resetLocale();

    Q_INVOKABLE void setPreferredSize( const QSize& );
    Q_INVOKABLE QSize preferredSize() const;

    Q_INVOKABLE QSize sizeConstraint() const;

    Q_INVOKABLE void setFixedSize( const QSize& );

    void polishItems();

    void setCustomRenderMode( const char* mode );
    const char* customRenderMode() const;

    // extra flag to interprete accepted events
    void setEventAcceptance( EventAcceptance );
    EventAcceptance eventAcceptance() const;

  Q_SIGNALS:
    void localeChanged( const QLocale& );
    void autoLayoutChildrenChanged();
    void deleteOnCloseChanged();

  public Q_SLOTS:
    void setLocale( const QLocale& );
    void resizeF( const QSizeF& );

  protected:
    bool event( QEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;
    void exposeEvent( QExposeEvent* ) override;
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    virtual void layoutItems();
    virtual void ensureFocus( Qt::FocusReason );

  private:
    void enforceSkin();

    Q_DECLARE_PRIVATE( QskWindow )
};

#endif
