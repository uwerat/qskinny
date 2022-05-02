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

    Q_PROPERTY( int count READ count NOTIFY countChanged )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged )

    using Inherited = QskPopup;

  public:
    QSK_SUBCONTROLS( Panel, Segment, Cursor, Text, Graphic, Separator )
    QSK_STATES( Selected )

    QskMenu( QQuickItem* parentItem = nullptr );
    ~QskMenu() override;

    bool isCascading() const;
    void setCascading( bool );
    void resetCascading();

    void setOrigin( const QPointF& );
    QPointF origin() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    // insert, remove, functors, actions, QskGraphic ...
    void addOption( const QUrl& graphicSource, const QString& text );
    void addOption( const QString& graphicSource, const QString& text );

    QVariantList optionAt( int ) const;
    int count() const;

    void addSeparator();

    int separatorPosition( int ) const;
    int separatorCount() const;

    void clear();

    int currentIndex() const;

    QRectF focusIndicatorRect() const override;

    QRectF cellRect( int index ) const;
    int indexAtPosition( const QPointF& ) const;

    Q_INVOKABLE int exec();

  Q_SIGNALS:
    void cascadingChanged( bool );
    void originChanged( const QPointF& );

    void triggered( int index );
    void currentIndexChanged( int index );

    void countChanged( int );

  public Q_SLOTS:
    void setCurrentIndex( int index );

  protected:
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    void wheelEvent( QWheelEvent* ) override;
#endif

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
