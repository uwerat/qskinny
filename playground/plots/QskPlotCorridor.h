/******************************************************************************
 * QSkinny - Copyright (C) The authors
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "QskPlotItem.h"
#include <qvector.h>

class QskPlotCorridorSample;
class QskPlotCorridorData;
class QColor;

// Only horizontal: TODO
class QskPlotCorridor : public QskPlotItem
{
    Q_OBJECT

    using Inherited = QskPlotItem;

    Q_PROPERTY( qreal borderWidth READ borderWidth
        WRITE setBorderWidth NOTIFY borderWidthChanged )

    Q_PROPERTY( QColor borderColor READ borderColor
        WRITE setBorderColor NOTIFY borderColorChanged )

    Q_PROPERTY( QColor color READ color
        WRITE setColor NOTIFY colorChanged )

  public:
    QSK_SUBCONTROLS( Border, Corridor )

    QskPlotCorridor( QObject* = nullptr );
    ~QskPlotCorridor() override;

    void setBorderWidth( qreal );
    qreal borderWidth() const;

    void setBorderColor( const QColor& );
    QColor borderColor() const;

    void setColor( const QColor& );
    QColor color() const;

    void setSamples( const QVector< QskPlotCorridorSample >& );

    void setData( QskPlotCorridorData* );
    QskPlotCorridorData* data() const;

    QskPlotCorridorSample interpolatedSample( qreal value ) const;

    void transformationChanged( ChangeFlags ) override;
    bool needsClipping() const override;

    using QskSkinnable::setColor;
    using QskSkinnable::color;

  Q_SIGNALS:
    void borderWidthChanged( qreal );
    void borderColorChanged( const QColor& );
    void colorChanged( const QColor& );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
