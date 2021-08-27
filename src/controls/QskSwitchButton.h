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

  public:
    QSK_SUBCONTROLS( Groove, Handle, Ripple )

    QskSwitchButton( Qt::Orientation, QQuickItem* parent = nullptr );
    QskSwitchButton( QQuickItem* parent = nullptr );

    ~QskSwitchButton() override;

    bool isCheckable() const override final;

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation);

    bool isInverted() const;
    void setInverted( bool );

    QskAspect::Placement effectivePlacement() const override;

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );
    void invertedChanged( bool );

  private:
    struct PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
