/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SEGMENTED_BAR_H
#define QSK_SEGMENTED_BAR_H

#include "QskControl.h"

#include <qurl.h>
#include <qstring.h>

class QskTextOptions;
class QskLabelData;

class QSK_EXPORT QskSegmentedBar : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( QVector< QskLabelData > options READ options
        WRITE setOptions NOTIFY optionsChanged )

    Q_PROPERTY( int count READ count )

    Q_PROPERTY( int selectedIndex READ selectedIndex
        WRITE setSelectedIndex NOTIFY selectedIndexChanged USER true )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged )

    Q_PROPERTY( QString currentText READ currentText )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Segment, Separator, Cursor, Text, Icon )
    QSK_STATES( Selected, Minimum, Maximum )

    QskSegmentedBar( QQuickItem* parent = nullptr );
    QskSegmentedBar( Qt::Orientation, QQuickItem* parent = nullptr );

    ~QskSegmentedBar() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    int addOption( const QUrl&, const QString& );
    int addOption( const QskLabelData& );

    void setOptions( const QVector< QskLabelData >& );

    QVector< QskLabelData > options() const;
    QskLabelData optionAt( int ) const;

    void clear();

    int selectedIndex() const;
    int currentIndex() const;

    QString currentText() const;

    int count() const;

    void setSegmentEnabled( int, bool );
    bool isSegmentEnabled( int ) const;

    int indexAtPosition( const QPointF& ) const;

    QRectF focusIndicatorRect() const override final;
    QskAspect::Variation effectiveVariation() const override;

  public Q_SLOTS:
    void setSelectedIndex( int index );
    void setCurrentIndex( int index );

  Q_SIGNALS:
    void selectedIndexChanged( int );
    void currentIndexChanged( int );
    void optionsChanged();
    void orientationChanged();

  protected:
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void focusInEvent( QFocusEvent* ) override;

  private:
    int nextIndex( int index, bool forward ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
