/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MENU_H
#define QSK_MENU_H

#include "QskPopup.h"

#include <qurl.h>
#include <qstring.h>

class QskTextOptions;
class QskGraphic;

class QSK_EXPORT QskMenu : public QskPopup
{
    Q_OBJECT

    Q_PROPERTY( QPointF origin READ origin
        WRITE setOrigin NOTIFY originChanged )

    Q_PROPERTY( bool cascading READ isCascading WRITE setCascading
        RESET resetCascading NOTIFY cascadingChanged )

    Q_PROPERTY( int count READ count )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged )

    using Inherited = QskPopup;

  public:
    struct Entry
    {
        QUrl iconSource;
        QString text;
    };

    QSK_SUBCONTROLS( Panel, Cell, Cursor, Text, Graphic )
    QSK_STATES( Selected )

    QskMenu( QQuickItem* parentItem = nullptr );
    ~QskMenu() override;

    bool isCascading() const;
    void setCascading( bool );
    void resetCascading();

    void setOrigin( const QPointF& );
    QPointF origin() const;

    // insert, remove, functors, actions, QskGraphic ...
    void addItem( const QUrl& graphicSource, const QString& text );
    void addItem( const QString& graphicSource, const QString& text );

    void addSeparator();

    Entry entryAt( int index ) const;
    QskGraphic graphicAt( int index ) const;

    void setTextOptions( const QskTextOptions& textOptions );
    QskTextOptions textOptions() const;

    int currentIndex() const;
    int count() const;

    virtual QskColorFilter graphicFilterAt( int index ) const;
    QRectF focusIndicatorRect() const override;

    QRectF cellRect( int index ) const;
    int indexAtPosition( const QPointF& ) const;

  Q_SIGNALS:
    void cascadingChanged( bool );
    void originChanged( const QPointF& );

    void triggered( int index );
    void currentIndexChanged( int index );

  public Q_SLOTS:
    void setCurrentIndex( int index );
    void clear();

  protected:
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;
    void wheelEvent( QWheelEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void aboutToShow() override;

  private:
    void traverse( int steps );
    void setSelectedIndex( int index );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
