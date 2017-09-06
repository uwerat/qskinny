/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LineEdit.h"

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskInputPanel.h>
#include <QskDialog.h>
#include <QskFocusIndicator.h>
#include <QskLinearBox.h>
#include <QskListView.h>

#include <QskWindow.h>
#include <QskSetup.h>
#include <QskAspect.h>

#include <QskObjectCounter.h>

#include <QGuiApplication>
#include <QFontMetricsF>

#define STRINGIFY(x) #x
#define STRING(x) STRINGIFY(x)

#define LOCAL_PANEL 1

class InputBox : public QskLinearBox
{
public:
    InputBox( QQuickItem* parentItem = nullptr ):
        QskLinearBox( Qt::Vertical, parentItem )
    {
        setDefaultAlignment( Qt::AlignHCenter | Qt::AlignTop );

        setMargins( 10 );
        setSpacing( 10 );

        auto* lineEdit = new LineEdit( this );

        lineEdit->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
        lineEdit->setBackgroundColor( Qt::white );
        lineEdit->setText( "I am a line edit. Press and edit Me." );

#if LOCAL_PANEL
        auto* inputPanel = new QskInputPanel( this );

        /*
            QskInputContext is connected to QskSetup::inputPanelChanged,
            making it the system input. If no input panel has been assigned
            QskInputContext would create a window or subwindow on the fly.
         */
        qskSetup->setInputPanel( inputPanel );
#endif
    }
};

class LocaleListView : public QskListView
{
public:
    LocaleListView( QQuickItem* parentItem = nullptr ):
        QskListView( parentItem ),
        m_maxWidth( 0.0 )
    {
        append( QLocale::Bulgarian, QStringLiteral( "български език" ) );
        append( QLocale::Czech, QStringLiteral( "Čeština" ) );
        append( QLocale::German, QStringLiteral( "Deutsch" ) );
        append( QLocale::Danish, QStringLiteral( "Dansk" ) );

        append( QLocale( QLocale::English, QLocale::UnitedStates ), QStringLiteral( "English (US)" ) );
        append( QLocale( QLocale::English, QLocale::UnitedKingdom ), QStringLiteral( "English (UK)" ) );

        append( QLocale::Spanish, QStringLiteral( "Español" ) );
        append( QLocale::Finnish, QStringLiteral( "Suomi" ) );
        append( QLocale::French, QStringLiteral( "Français" ) );
        append( QLocale::Hungarian, QStringLiteral( "Magyar" ) );
        append( QLocale::Italian, QStringLiteral( "Italiano" ) );
        append( QLocale::Japanese, QStringLiteral( "日本語" ) );
        append( QLocale::Latvian, QStringLiteral( "Latviešu" ) );
        append( QLocale::Lithuanian, QStringLiteral( "Lietuvių") );
        append( QLocale::Dutch, QStringLiteral( "Nederlands" ) );
        append( QLocale::Portuguese, QStringLiteral( "Português" ) );
        append( QLocale::Romanian, QStringLiteral( "Română" ) );
        append( QLocale::Russian, QStringLiteral( "Русский" ) );
        append( QLocale::Slovenian, QStringLiteral( "Slovenščina" ) );
        append( QLocale::Slovak, QStringLiteral( "Slovenčina" ) );
        append( QLocale::Turkish, QStringLiteral( "Türkçe" ) );
        append( QLocale::Chinese, QStringLiteral( "中文" ) );

        setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
        setPreferredWidth( columnWidth( 0 ) + 20 );

        setScrollableSize( QSizeF( columnWidth( 0 ), rowCount() * rowHeight() ) );

        // TODO:
        // 1) changing the keyboard layouts does not yet work
        // 2) QskInputPanel does not work properly in the threaded environment
#if 1
        connect( this, &QskListView::selectedRowChanged,
            this, [ = ] { qskSetup->inputPanel()->setLocale( m_values[selectedRow()].second ); } );
#else
        connect( this, &QskListView::selectedRowChanged,
            this, [ = ] { QLocale::setDefault( m_values[selectedRow()].second ); } );
#endif
    }

    virtual int rowCount() const override final
    {
        return m_values.count();
    }

    virtual int columnCount() const override final
    {
        return 1;
    }

    virtual qreal columnWidth( int ) const override
    {
        if ( m_maxWidth == 0.0 )
        {
            using namespace QskAspect;

            const QFontMetricsF fm( effectiveFont( Text ) );

            for ( auto entry : m_values )
                m_maxWidth = qMax( m_maxWidth, fm.width( entry.first ) );

            const QMarginsF padding = marginsHint( Cell | Padding );
            m_maxWidth += padding.left() + padding.right();
        }

        return m_maxWidth;
    }

    virtual qreal rowHeight() const override
    {
        using namespace QskAspect;

        const QFontMetricsF fm( effectiveFont( Text ) );
        const QMarginsF padding = marginsHint( Cell | Padding );

        return fm.height() + padding.top() + padding.bottom();
    }

    virtual QVariant valueAt( int row, int ) const override final
    {
        return m_values[row].first;
    }

private:
    inline void append( QLocale locale, const QString& name )
    {
        m_values += qMakePair( QString( name ), locale );
    }

    QVector< QPair< QString, QLocale > > m_values;
    mutable qreal m_maxWidth;
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    qputenv( "QT_IM_MODULE", "skinny" );
    qputenv( "QT_PLUGIN_PATH", STRING( PLUGIN_PATH ) );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

#if !LOCAL_PANEL
    // We don't want to have a top level window.
    qskDialog->setPolicy( QskDialog::EmbeddedBox );
#endif

    auto box = new QskLinearBox( Qt::Horizontal );
    box->setSpacing( 10 );
    box->setMargins( 20 );

    (void) new LocaleListView( box );
    (void) new InputBox( box );

    QskWindow window;
    window.setColor( "PapayaWhip" );
    window.addItem( box );
    window.addItem( new QskFocusIndicator() );

    window.resize( 800, 300 );
    window.show();

    return app.exec();
}
