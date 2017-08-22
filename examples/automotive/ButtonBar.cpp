#include "ButtonBar.h"
#include "SkinFactory.h"
#include <QskGraphic.h>
#include <QskGraphicIO.h>
#include <QskGraphicLabel.h>

QSK_SUBCONTROL( ButtonBar, Indicator )

class IndicatorLabel : public QskGraphicLabel
{
public:
    IndicatorLabel( QQuickItem* parentItem = nullptr ):
        QskGraphicLabel( parentItem )
    {
        // so the skins are able to colorize them
        setGraphicRole( SkinFactory::Indicator );
    }

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override final
    {
        // so that we can set specific colors in the skin

        if ( subControl == QskGraphicLabel::Graphic )
            return ButtonBar::Indicator;

        return subControl;
    }
};

ButtonBar::ButtonBar( QQuickItem* parentItem ):
    QskLinearBox( parentItem )
{
    QColor c( Qt::black );
    c.setAlphaF( 0.5 );
    setBackgroundColor( c );

    setMargins( QMarginsF( 20, 15, 20, 15 ) );
    setSpacing( 20 );

    setSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::MinimumExpanding );
}

void ButtonBar::addIndicator( const char* name )
{
    auto* label = new IndicatorLabel( this );

    /*
        The label should adjust vertically and be stretched horizontally
        according to its aspect ratio.
     */

    label->setSizePolicy( QskSizePolicy::Constrained, QskSizePolicy::Ignored );

    const QString fileName = QString( ":/qvg/%1.qvg" ).arg( name );
    label->setGraphic( QskGraphicIO::read( fileName ) );
}

QSizeF ButtonBar::contentsSizeHint() const
{
    return QSizeF( -1, 20 );
}
