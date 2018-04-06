/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskVirtualKeyboard.h>
#include <QskDialog.h>
#include <QskFocusIndicator.h>
#include <QskLinearBox.h>
#include <QskListView.h>
#include <QskTextInput.h>
#include <QskInputPanel.h>

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

        auto* textInput = new QskTextInput( this );
        textInput->setText( "I am a line edit. Press and edit Me." );
        textInput->setSelectByMouse( true );

#if LOCAL_PANEL
        auto* inputPanel = new QskVirtualKeyboard( this );

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
        for ( auto language :
            {
                QLocale::Bulgarian, QLocale::Czech, QLocale::German,
                QLocale::Danish, QLocale::English, QLocale::Spanish,
                QLocale::Finnish, QLocale::French, QLocale::Hungarian,
                QLocale::Italian, QLocale::Japanese, QLocale::Latvian,
                QLocale::Lithuanian, QLocale::Dutch, QLocale::Portuguese,
                QLocale::Romanian, QLocale::Russian, QLocale::Slovenian,
                QLocale::Slovak, QLocale::Turkish, QLocale::Chinese
            } )
        {

            if ( language == QLocale::English )
            {
                append( QLocale( QLocale::English, QLocale::UnitedStates ) );
                append( QLocale( QLocale::English, QLocale::UnitedKingdom ) );
            }
            else
            {
                append( QLocale( language ) );
            }
        }

        setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
        setPreferredWidth( columnWidth( 0 ) + 20 );

        setScrollableSize( QSizeF( columnWidth( 0 ), rowCount() * rowHeight() ) );
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

            for ( const auto& entry : m_values )
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

    QLocale localeAt( int row ) const
    {
        if ( row >= 0 && row < m_values.size() )
            return m_values[row].second;

        return QLocale();
    }

private:
    inline void append( const QLocale& locale )
    {
        m_values += qMakePair( qskNativeLocaleString( locale ), locale );
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

    auto listView = new LocaleListView( box );
    auto inputBox =  new InputBox( box );

    /*
        Disable Qt::ClickFocus, so that the input panel stays open
        when selecting a different locale
     */
    listView->setFocusPolicy( Qt::TabFocus );

    QObject::connect( listView, &QskListView::selectedRowChanged,
        inputBox, [ = ]( int row ) { inputBox->setLocale( listView->localeAt( row ) ); } );

    QskWindow window;
    window.setColor( "PapayaWhip" );
    window.addItem( box );
    window.addItem( new QskFocusIndicator() );

    window.resize( 800, 300 );
    window.show();

    return app.exec();
}
