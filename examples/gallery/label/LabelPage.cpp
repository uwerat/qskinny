/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LabelPage.h"
#include <QskTextLabel.h>
#include <QskGraphicLabel.h>
#include <QskSeparator.h>
#include <QskRgbValue.h>

namespace
{
    class TextBox : public QskLinearBox
    {
      public:
        TextBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, 3, parent )
        {
            setMargins( 10 );
            //setDefaultAlignment( Qt::AlignTop );
            setExtraSpacingAt( Qt::BottomEdge );

            const QStringList texts =
                { "Default", "Tiny", "Small", "Medium", "Large", "Huge" };

            for ( int i = 0; i < texts.size(); i++ )
            {
                auto label = new QskTextLabel( texts[ i ] + " Font", this );

                //label->setPanel( true );
                label->setFontRole( i );
            }
        }
    };

    class IconBox : public QskLinearBox
    {
      public:
        IconBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 3, parent )
        {
            setMargins( 10 );
            setSpacing( 20 );

            const char* icons[] =
            {
                "rectangle/royalblue",
                "triangleright/thistle",
                "ellipse/khaki",
                "ring/sandybrown",
                "star/darkviolet",
                "hexagon/darkslategray"
            };

            const auto prefix = QStringLiteral( "image://shapes/" );

            for ( const auto icon : icons )
            {
                auto label = new QskGraphicLabel( prefix + icon, this );
                label->setAlignment( Qt::AlignCenter );
            }
        }
    };
}

LabelPage::LabelPage( QQuickItem* parent )
    : Page( Qt::Vertical, parent )
{
    setGradient( QskRgb::AliceBlue );
    setSpacing( 40 );

    (void) new TextBox( this );
    (void) new QskSeparator( this );
    (void) new IconBox( this );
}
