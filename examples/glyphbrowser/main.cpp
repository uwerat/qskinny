/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "GlyphListView.h"

#include <SkinnyShortcut.h>

#include <QskMainView.h>
#include <QskFocusIndicator.h>
#include <QskObjectCounter.h>
#include <QskWindow.h>
#include <QskComboBox.h>

#include <QGuiApplication>
#include <QRawFont>
#include <QDir>

namespace
{
    class Header : public QskLinearBox
    {
        Q_OBJECT

      public:
        Header( QQuickItem* parent = nullptr )
            : QskLinearBox( Qt::Horizontal, parent )
        {
            setPaddingHint( QskBox::Panel, 5 );

            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );
            setPanel( true );

            m_comboBox = new QskComboBox( this );

            const auto entries = QDir( ":iconfonts" ).entryInfoList();
            for ( const auto& entry : entries )
                m_comboBox->addOption( QUrl(), entry.absoluteFilePath() );

            m_comboBox->setCurrentIndex( 0 );

            connect( m_comboBox, &QskComboBox::currentIndexChanged,
                this, &Header::comboBoxChanged );
        }

        QString fontPath() const { return m_comboBox->currentText(); }

      Q_SIGNALS:
        void fontChanged( const QString& );

      private:
        void comboBoxChanged() { Q_EMIT fontChanged( fontPath() ); }

        QskComboBox* m_comboBox;
    };

    class MainView : public QskMainView
    {
      public:
        MainView( QQuickItem* parent = nullptr )
            : QskMainView( parent )
        {
            auto listView = new GlyphListView( this );
            auto header = new Header( this );

            setHeader( header );
            setBody( listView );

            listView->setFontPath( header->fontPath() );

            connect( header, &Header::fontChanged,
                listView, &GlyphListView::setFontPath );
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

    auto mainView = new QskMainView();
    mainView->setBody( new GlyphListView( ":/fonts/Octicons.ttf" ) );

    QskWindow window;
    window.addItem( new MainView() );
    window.addItem( new QskFocusIndicator() );
    window.resize( 600, 400 );
    window.show();

    return app.exec();
}

#include "main.moc"
