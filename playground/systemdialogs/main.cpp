/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyShortcut.h>

#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskPushButton.h>
#include <QskCheckBox.h>
#include <QskLinearBox.h>
#include <QskMainView.h>
#include <QskDialog.h>

#include <QGuiApplication>

#include <private/qquickcolordialog_p.h>
#include <private/qquickfiledialog_p.h>
#include <private/qquickfolderdialog_p.h>
#include <private/qquickfontdialog_p.h>
#include <private/qquickmessagedialog_p.h>

namespace
{
    class ButtonBox : public QskLinearBox
    {
        Q_OBJECT

      public:
        enum DialogType
        {
            ColorDialog,
            FileDialog,
            FolderDialog,
            FontDialog,
            MessageDialog,
        };
        Q_ENUM( DialogType )

        ButtonBox( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Vertical, parent )
        {
            setDefaultAlignment( Qt::AlignCenter );

            setMargins( 10 );
            setSpacing( 20 );

            const auto metaEnum = QMetaEnum::fromType<DialogType>();

            for ( int i = ColorDialog; i <= MessageDialog; i++ )
            {
                auto button = new QskPushButton( metaEnum.key( i ), this  );
                button->setPreferredWidth( 200 );
                button->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

                connect( button, &QskPushButton::clicked,
                    this, [ this, i ]() { openDialog( i ); } );
            }

            setExtraSpacingAt( Qt::BottomEdge );
        }

      private:
        void openDialog( int dialogType )
        {
            /*
                Qt/Quick Dialogs implements a thin wrapper to make
                QPlatformTheme::createPlatformDialogHelper accessible from QML.
                There is not much value for the QSkinny use case and we could
                use QPlatformTheme in our QskDialog classes without the wrapper.

                However Qt/Quick Dialogs also offers a fallback implementation
                that is used when a dialog type is not supported by the platform.
                These classes are implemented in QML and we need QmlEngine/QmlComponent
                to use them. Once we have our own fallback implementation we can
                drop this QML dependency.
             */
            delete m_dialog;
            m_dialog = nullptr;

            if ( qGuiApp->testAttribute( Qt::AA_DontUseNativeDialogs ) )
            {
                switch( dialogType )
                {
                    case ColorDialog:
                    {
                        qskDialog->selectColor( "select color" );
                        break;
                    }
                    case FileDialog:
                    {
                        auto file = qskDialog->selectFile( "select file", QDir::currentPath() );
                        break;
                    }
                    case FolderDialog:
                    {
                        auto file = qskDialog->selectDirectory( "select directory", QDir::currentPath() );
                        break;
                    }
                    case FontDialog:
                    {
                        qskDialog->selectFont( "select font" );
                        break;
                    }
                    case MessageDialog:
                    {
                        auto action = qskDialog->message( "message", "The quick brown fox jumps over the lazy dog" );
                        qDebug() << "got message action" << action;
                        break;
                    }
                    default:
                    {
                        qWarning() << "unknown dialog type detected";
                    }
                }
            }
            else
            {
                switch( dialogType )
                {
                    case ColorDialog:
                        m_dialog = new QQuickColorDialog();
                        break;

                    case FileDialog:
                        m_dialog = new QQuickFileDialog();
                        break;

                    case FolderDialog:
                        m_dialog = new QQuickFolderDialog();
                        break;

                    case FontDialog:
                        m_dialog = new QQuickFontDialog();
                        break;

                    case MessageDialog:
                        m_dialog = new QQuickMessageDialog();
                        break;
                }
            }

            if ( m_dialog )
            {
                if ( auto messageDialog = qobject_cast< QQuickMessageDialog* >( m_dialog ) )
                    messageDialog->setText( "The quick brown fox jumps over the lazy dog" );
                    
                //m_dialog->setModality( Qt::WindowModal );
                m_dialog->open();
            }
        }

        QQuickAbstractDialog* m_dialog = nullptr;
    };

    class MainView : public QskMainView
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskMainView( parent )
        {
            auto header = new QskLinearBox();
            header->setExtraSpacingAt( Qt::LeftEdge );
            header->setSizePolicy( Qt::Vertical, QskSizePolicy::Fixed );

            auto button = new QskCheckBox( "Try Native Dialogs", header );
            button->setChecked( true );

            connect( button, &QskCheckBox::toggled,
                []( bool on ) { qGuiApp->setAttribute( Qt::AA_DontUseNativeDialogs, !on ); } );

            setHeader( header );
            setBody( new ButtonBox() );
        }
    };
}

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.addItem( new MainView() );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}

#include "main.moc"
