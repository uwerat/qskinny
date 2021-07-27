#include "QskSwitchButton.h"

QSK_SUBCONTROL( QskSwitchButton, Knop )
QSK_SUBCONTROL( QskSwitchButton, Groove )

struct QskSwitchButton::PrivateData
{
    PrivateData()
        : orientation( Qt::Horizontal )
        , layoutDirection( Qt::LeftToRight)
    {
    }

    Qt::Orientation orientation;
    Qt::LayoutDirection layoutDirection;
};

QskSwitchButton::QskSwitchButton( QQuickItem* parent )
    : QskAbstractButton(parent)
    , m_data( new PrivateData() )
{
    setCheckable( true );
}

QskSwitchButton::~QskSwitchButton() {
}


Qt::Orientation QskSwitchButton::orientation() const
{
    return m_data->orientation;
}
void QskSwitchButton::setOrientation(Qt::Orientation orientation)
{
    if(m_data->orientation != orientation)
    {
        m_data->orientation = orientation;
        update();
        Q_EMIT orientationChanged( orientation );
    }
}

Qt::LayoutDirection QskSwitchButton::layoutDirection() const
{
    return m_data->layoutDirection;
}
void QskSwitchButton::setLayoutDirection(Qt::LayoutDirection layoutDirection)
{
    if(m_data->layoutDirection != layoutDirection)
    {
        m_data->layoutDirection = layoutDirection;
        update();
        Q_EMIT layoutDirectionChanged( layoutDirection );
    }
}

#include "moc_QskSwitchButton.cpp"
