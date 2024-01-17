/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOX_H
#define QSK_BOX_H

#include "QskControl.h"

class QSK_EXPORT QskBox : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

    Q_PROPERTY( bool panel READ hasPanel
        WRITE setPanel NOTIFY panelChanged FINAL )

    Q_PROPERTY( QskBoxBorderMetrics borderMetrics READ borderMetrics
        WRITE setBorderMetrics RESET resetBorderMetrics
        NOTIFY borderMetricsChanged )

    Q_PROPERTY( QskBoxBorderColors borderColors READ borderColors
        WRITE setBorderColors RESET resetBorderColors
        NOTIFY borderColorsChanged )

    Q_PROPERTY( QskGradient setFillGradient READ fillGradient
        WRITE setFillGradient RESET resetFillGradient
        NOTIFY fillGradientChanged )

    Q_PROPERTY( QskMargins padding READ padding
        WRITE setPadding RESET resetPadding NOTIFY paddingChanged )

  public:
    QSK_SUBCONTROLS( Panel )

    QskBox( QQuickItem* parent = nullptr );
    QskBox( bool hasPanel, QQuickItem* parent = nullptr );

    ~QskBox() override;

    void setPanel( bool );
    bool hasPanel() const;

    QskBoxBorderMetrics borderMetrics() const;
    void setBorderMetrics( const QskBoxBorderMetrics& );
    void resetBorderMetrics();

    QskBoxBorderColors borderColors() const;
    void setBorderColors( const QskBoxBorderColors& );
    void resetBorderColors();

    QskGradient fillGradient() const;
    void setFillGradient( const QskGradient& );
    void resetFillGradient();

    void setPadding( qreal );
    void setPadding( const QMarginsF& );
    void resetPadding();
    QMarginsF padding() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;

  Q_SIGNALS:
    void panelChanged( bool );
    void borderMetricsChanged( const QskBoxBorderMetrics& );
    void borderColorsChanged( const QskBoxBorderColors& );
    void fillGradientChanged( const QskGradient& );
    void paddingChanged( const QMarginsF& );

  private:
    bool m_hasPanel;
};

#endif
