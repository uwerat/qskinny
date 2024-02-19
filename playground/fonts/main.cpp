/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyNamespace.h>
#include <SkinnyShortcut.h>

#include <QskFontRole.h>
#include <QskTextLabel.h>
#include <QskLinearBox.h>
#include <QskSeparator.h>
#include <QskWindow.h>
#include <QskSkinManager.h>

namespace
{
    class TextLabel : public QskTextLabel
    {
      public:
        TextLabel( const QskFontRole& role, QQuickItem* parent = nullptr )
            : QskTextLabel( parent )
        {
            setAlignment( Qt::AlignCenter );
            setText( textFromRole( role ) );
            setFontRole( role );

            setSizePolicy( Qt::Horizontal, QskSizePolicy::Ignored );
        }

      private:
        QString textFromRole( const QskFontRole& role ) const
        {
            static QMetaEnum metaEnums[2];

            const auto& mo = QskFontRole::staticMetaObject;

            if ( !metaEnums[0].isValid() )
                metaEnums[0] = mo.enumerator( mo.indexOfEnumerator( "Category" ) );

            if ( !metaEnums[1].isValid() )
                metaEnums[1] = mo.enumerator( mo.indexOfEnumerator( "Emphasis" ) );

            QString s( metaEnums[0].valueToKey( role.category() ) );
#if 0
            s += ' ';
            s += metaEnums[1].valueToKey( role.emphasis() );
#endif

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

            for ( int i = 0; i <= QskFontRole::Display; i++ )
            {
                const auto category = static_cast< QskFontRole::Category >( i );
                for ( int j = 0; j <= QskFontRole::VeryHigh; j++ )
                {
                    const auto emphasis = static_cast< QskFontRole::Emphasis >( j );
                    ( void ) new TextLabel( { category, emphasis }, this );
                }
            }
        }
    };

    class SkinLabel : public QskTextLabel
    {
      public:
        SkinLabel( QQuickItem* parent = nullptr )
            : QskTextLabel( parent )
        {
            setSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Fixed );
            setAlignment( Qt::AlignCenter );
            setFontRole( QskFontRole::Title );

            updateText();
        }

      protected:
        void changeEvent( QEvent* event )
        {
            if ( event->type() == QEvent::StyleChange )
                updateText();

            QskTextLabel::changeEvent( event );
        }

      private:
        void updateText()
        {
            setText( qskSkinManager->skinName() );
        }
    };

    class MainView : public QskLinearBox
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setPanel( true );

            (void) new SkinLabel( this );
            (void) new QskSeparator( this );
            (void) new TextBox( this );
        }
    };
}

int main( int argc, char** argv )
{
    QGuiApplication app( argc, argv );

    Skinny::init(); // we need a skin
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.addItem( new MainView() );
    window.resize( 1024, 600 );
    window.show();

    return app.exec();
}
