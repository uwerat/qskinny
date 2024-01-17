/******************************************************************************
 * QSkinny - Copyright (C) The authors
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "QskPlotItem.h"

#include <qnamespace.h>
#include <qvector.h>
#include <qpen.h>

class QPen;

class QskPlotGrid : public QskPlotItem
{
    Q_OBJECT

    Q_PROPERTY( QPen minorPen READ minorPen
        WRITE setMinorPen RESET resetMinorPen NOTIFY minorPenChanged )

    Q_PROPERTY( QPen majorPen READ majorPen
        WRITE setMajorPen RESET resetMajorPen NOTIFY majorPenChanged )

    using Inherited = QskPlotItem;

  public:
    QSK_SUBCONTROLS( MajorLine, MinorLine )

    enum Type
    {
        MinorGrid,
        MajorGrid
    };
    Q_ENUM( Type )

    QskPlotGrid( QObject* = nullptr );
    ~QskPlotGrid() override;

    void setPen( Type, const QPen& );
    void resetPen( Type );
    QPen pen( Type ) const;

    void setMajorPen( const QPen& );
    void resetMajorPen();
    QPen majorPen() const;

    void setMinorPen( const QPen& );
    void resetMinorPen();
    QPen minorPen() const;

    // positions
    virtual QVector< qreal > lines( Type, Qt::Orientation ) const;

    bool needsClipping() const override;

  Q_SIGNALS:
    void minorPenChanged( const QPen& );
    void majorPenChanged( const QPen& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline void QskPlotGrid::setMinorPen( const QPen& pen )
{
    setPen( MinorGrid, pen );
}

inline void QskPlotGrid::resetMinorPen()
{
    resetPen( MinorGrid );
}

inline QPen QskPlotGrid::minorPen() const
{
    return pen( MinorGrid );
}

inline void QskPlotGrid::setMajorPen( const QPen& pen )
{
    setPen( MajorGrid, pen );
}

inline void QskPlotGrid::resetMajorPen()
{
    resetPen( MajorGrid );
}

inline QPen QskPlotGrid::majorPen() const
{
    return pen( MajorGrid );
}


