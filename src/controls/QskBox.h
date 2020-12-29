/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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

    Q_PROPERTY( QskMargins padding READ padding
        WRITE setPadding RESET resetPadding NOTIFY paddingChanged )

  public:
    QSK_SUBCONTROLS( Panel )

    QskBox( QQuickItem* parent = nullptr );
    QskBox( bool hasPanel, QQuickItem* parent = nullptr );

    ~QskBox() override;

    void setPanel( bool );
    bool hasPanel() const;

    void setPadding( qreal );
    void setPadding( const QMarginsF& );
    void resetPadding();
    QMarginsF padding() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;

  Q_SIGNALS:
    void panelChanged( bool );
    void paddingChanged( const QMarginsF& );

  private:
    bool m_hasPanel;
};

#endif
