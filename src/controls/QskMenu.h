/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MENU_H
#define QSK_MENU_H

#include "QskPopup.h"
#include <qstringlist.h>

class QskTextOptions;
class QskLabelData;
class QUrl;

class QSK_EXPORT QskMenu : public QskPopup
{
    Q_OBJECT

    Q_PROPERTY( QPointF origin READ origin
        WRITE setOrigin NOTIFY originChanged )

    Q_PROPERTY( bool cascading READ isCascading WRITE setCascading
        RESET resetCascading NOTIFY cascadingChanged )

    Q_PROPERTY( QVector< QskLabelData > options READ options
        WRITE setOptions NOTIFY optionsChanged )

    Q_PROPERTY( int count READ count )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged )

    Q_PROPERTY( int triggeredIndex READ triggeredIndex NOTIFY triggered )

    Q_PROPERTY( QString triggeredText READ triggeredText NOTIFY triggered )
    Q_PROPERTY( QString currentText READ currentText NOTIFY currentIndexChanged )

    using Inherited = QskPopup;

  public:
    QSK_SUBCONTROLS( Overlay, Panel, Segment, Cursor, Text, Icon, Separator )
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

    int addOption( const QString&, const QString& );
    int addOption( const QUrl&, const QString& );
    int addOption( const QskLabelData& );

    void setOptions( const QVector< QskLabelData >& );
    void setOptions( const QStringList& );

    QVector< QskLabelData > options() const;
    QskLabelData optionAt( int ) const;

    int count() const;

    void addSeparator();

    int separatorPosition( int ) const;
    int separatorCount() const;

    void clear();

    int currentIndex() const;
    QString currentText() const;

    int triggeredIndex() const;
    QString triggeredText() const;

    QRectF focusIndicatorRect() const override;

    QRectF cellRect( int index ) const;
    int indexAtPosition( const QPointF& ) const;

    Q_INVOKABLE int exec();

  Q_SIGNALS:
    void cascadingChanged( bool );
    void originChanged( const QPointF& );

    void triggered( int index );
    void currentIndexChanged( int );

    void optionsChanged();

  public Q_SLOTS:
    void setCurrentIndex( int );

  protected:
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    void wheelEvent( QWheelEvent* ) override;
#endif

    void mousePressEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void aboutToShow() override;
    void trigger( int );

  private:
    void traverse( int steps );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
