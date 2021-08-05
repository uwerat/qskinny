/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskBox.h>

class QskGraphicLabel;

class RoundedIcon : public QskBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, Icon )
    QSK_STATES( Bright, Small ) // to differentiate between orange and purple and small vs. big

    RoundedIcon( const QString& iconName, bool isBright, bool isSmall,
                 QQuickItem* parent = nullptr );

  protected:
    void updateLayout() override;
    virtual QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    QskGraphicLabel* m_graphicLabel = nullptr;
};
