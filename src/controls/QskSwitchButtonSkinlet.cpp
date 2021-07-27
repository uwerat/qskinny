
#include "QskSwitchButton.h"
#include "QskSwitchButtonSkinlet.h"
#include "QskSGNode.h"
QskSwitchButtonSkinlet::QskSwitchButtonSkinlet(QskSkin* skin)
    : Inherited( skin )
{
    setNodeRoles( { GrooveRole, KnopRole } );
}

QskSwitchButtonSkinlet::~QskSwitchButtonSkinlet() {}

QRectF QskSwitchButtonSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl) const
{
    using Q = QskSwitchButton;
    const auto switchButton = static_cast< const Q* >( skinnable );

    if (!switchButton)
    {
        return Inherited::subControlRect( skinnable, contentsRect, subControl );
    }

    if ( subControl == Q::Knop)
    {
        const auto diameter = 2 * skinnable->metric(QskSwitchButton::Knop | QskAspect::Size);
        const auto grooveSize = skinnable->strutSizeHint(QskSwitchButton::Groove);
        auto position = skinnable->metric( Q::Knop | QskAspect::Position );

        auto rect = QRectF(0, 0, diameter, diameter);

        if(switchButton->layoutDirection() == Qt::RightToLeft)
        {
            position = 1 - position;
        }

        if(switchButton->orientation() == Qt::Vertical)
        {
            if(diameter < grooveSize.height() )
            {
                rect.moveLeft( ( grooveSize.height() - diameter ) / 2);
            }
            rect.moveTop( ( grooveSize.height() - diameter ) / 2
                + position * ( grooveSize.width() - diameter
                - ( grooveSize.height() - diameter ) ) );
        }
        else
        {
            if(diameter < grooveSize.height() )
            {
                rect.moveTop( ( grooveSize.height() - diameter ) / 2);
            }
            rect.moveLeft( ( grooveSize.height() - diameter ) / 2
                + position * ( grooveSize.width() - diameter
                - ( grooveSize.height() - diameter ) ) );
        }

        return rect;
    }
    else if ( subControl == Q::Groove )
    {
        auto diameter = 2 * skinnable->metric(QskSwitchButton::Knop | QskAspect::Size);
        const auto grooveSize = skinnable->strutSizeHint(QskSwitchButton::Groove);
        auto result = contentsRect;
        result.setSize( QSizeF(grooveSize.width(), grooveSize.height() ) );

        if(switchButton->orientation() == Qt::Vertical )
        {
            if(grooveSize.height() < diameter)
            {
                result.moveLeft( ( diameter - result.height() ) / 2);
            }
            return result.transposed();
        }
        else
        {
            if(grooveSize.height() < diameter)
            {
                result.moveTop( ( diameter - result.height() ) / 2);
            }
            return result;
        }
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSizeF QskSwitchButtonSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint, const QSizeF&) const
{
    const auto switchButton = static_cast< const QskSwitchButton* >( skinnable );
    const auto diameter = 2 * skinnable->metric(QskSwitchButton::Knop | QskAspect::Size);
    const auto grooveSize = skinnable->strutSizeHint(QskSwitchButton::Groove);

    auto result = QSizeF(qMax(diameter, grooveSize.width() ),
        qMax(diameter, grooveSize.height() ) );
    if(switchButton->orientation() == Qt::Vertical)
    {
        return result.transposed();
    }

    return result;
}

QSGNode* QskSwitchButtonSkinlet::updateSubNode( const QskSkinnable* skinnable,
    quint8 nodeRole, QSGNode* node) const
{
    const auto switchButton = static_cast< const QskSwitchButton* >( skinnable );

    switch ( nodeRole )
    {
        case KnopRole:
            return updateBoxNode( switchButton, node, QskSwitchButton::Knop );

        case GrooveRole:
            return updateBoxNode( switchButton, node, QskSwitchButton::Groove );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}
