#ifndef QSK_SWITCH_BUTTON_H
#define QSK_SWITCH_BUTTON_H


#include "QskAbstractButton.h"
#include "QskNamespace.h"

class QSK_EXPORT QskSwitchButton : public QskAbstractButton
{
    Q_OBJECT

    using Inherited = QskAbstractButton;

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged FINAL )
    Q_PROPERTY( Qt::LayoutDirection layoutDirection READ layoutDirection
        WRITE setLayoutDirection NOTIFY layoutDirectionChanged FINAL )

  public:
    QSK_SUBCONTROLS( Groove, Knop )

    QskSwitchButton( QQuickItem* parent = nullptr );
    ~QskSwitchButton() override;

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection);

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );
    void layoutDirectionChanged(Qt::LayoutDirection);

  private:
    struct PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
#endif
