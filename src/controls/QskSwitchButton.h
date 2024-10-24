/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SWITCH_BUTTON_H
#define QSK_SWITCH_BUTTON_H

#include "QskAbstractButton.h"
#include <qnamespace.h>

class QSK_EXPORT QskSwitchButton : public QskAbstractButton
{
    Q_OBJECT

    using Inherited = QskAbstractButton;

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged FINAL )

    Q_PROPERTY( bool inverted READ isInverted
        WRITE setInverted NOTIFY invertedChanged FINAL )

    Q_PROPERTY( IconMode iconMode READ iconMode
        WRITE setIconMode NOTIFY iconModeChanged FINAL )

  public:
    QSK_SUBCONTROLS( Groove, Handle, Icon )

    enum IconMode
    {
        NoIcon,
        ShowIconWhenSelected,
        ShowIconAlways
    };
    Q_ENUM( IconMode )

    QskSwitchButton( Qt::Orientation, QQuickItem* parent = nullptr );
    QskSwitchButton( QQuickItem* parent = nullptr );

    ~QskSwitchButton() override;

    bool isCheckable() const override final;

    Qt::Orientation orientation() const;
    void setOrientation( Qt::Orientation );

    bool isInverted() const;
    void setInverted( bool );

    IconMode iconMode() const;
    void setIconMode( IconMode );

    QskAspect::Variation effectiveVariation() const override;

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );
    void invertedChanged( bool );
    void iconModeChanged( IconMode );

  private:
    struct PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
