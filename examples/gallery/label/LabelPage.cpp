/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LabelPage.h"
#include <QskTextLabel.h>
#include <QskGraphicLabel.h>
#include <QskSeparator.h>
#include <QskRgbValue.h>
#include <QskSkin.h>

namespace
{
    class TextLabel : public QskTextLabel
    {
      public:
        TextLabel( int role, QQuickItem* parent = nullptr )
            : QskTextLabel( parent )
        {
            setText( textFromRole( role ) );
            setFontRole( role );

            setSizePolicy( Qt::Horizontal, QskSizePolicy::Ignored );
        }

      private:
        QString textFromRole( int role ) const
        {
            static QMetaEnum metaEnum;

            if ( !metaEnum.isValid() )
            {
                const auto& mo = QskSkin::staticMetaObject;
                metaEnum = mo.enumerator( mo.indexOfEnumerator( "SkinFontRole" ) );
            }

            QString s( metaEnum.valueToKey( role ) );
            s.remove( QStringLiteral( "Font" ) );

            return s;
        }
    };

    class TextBox : public QskLinearBox
    {
      public:
        TextBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, 3, parent )
        {
            setMargins( 10 );
            setDefaultAlignment( Qt::AlignCenter );

            for ( int i = 0; i <= QskSkin::HugeFont; i++ )
                ( void ) new TextLabel( i, this );
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
    setSpacing( 40 );

    (void) new TextBox( this );
    (void) new QskSeparator( this );
    (void) new IconBox( this );
}
