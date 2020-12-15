/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskDialog.h>
#include <QskFocusIndicator.h>
#include <QskInputContext.h>
#include <QskInputPanel.h>
#include <QskLinearBox.h>
#include <QskListView.h>
#include <QskTextInput.h>

#include <QskAspect.h>
#include <QskWindow.h>
#include <QskFunctions.h>

#include <QskObjectCounter.h>

#include <QFontMetricsF>
#include <QGuiApplication>

static inline QString nativeLocaleString( const QLocale& locale )
{
    switch ( locale.language() )
    {
        case QLocale::Bulgarian:
            return QStringLiteral( "български език" );

        case QLocale::Czech:
            return QStringLiteral( "Čeština" );

        case QLocale::German:
            return QStringLiteral( "Deutsch" );

        case QLocale::Danish:
            return QStringLiteral( "Dansk" );

        case QLocale::Greek:
            return QStringLiteral( "Eλληνικά" );

        case QLocale::English:
        {
            switch ( locale.country() )
            {
                case QLocale::Canada:
                case QLocale::UnitedStates:
                case QLocale::UnitedStatesMinorOutlyingIslands:
                case QLocale::UnitedStatesVirginIslands:
                    return QStringLiteral( "English (US)" );

                default:
                    return QStringLiteral( "English (UK)" );
            }
        }

        case QLocale::Spanish:
            return QStringLiteral( "Español" );

        case QLocale::Finnish:
            return QStringLiteral( "Suomi" );

        case QLocale::French:
            return QStringLiteral( "Français" );

        case QLocale::Hungarian:
            return QStringLiteral( "Magyar" );

        case QLocale::Italian:
            return QStringLiteral( "Italiano" );

        case QLocale::Japanese:
            return QStringLiteral( "日本語" );

        case QLocale::Latvian:
            return QStringLiteral( "Latviešu" );

        case QLocale::Lithuanian:
            return QStringLiteral( "Lietuvių" );

        case QLocale::Dutch:
            return QStringLiteral( "Nederlands" );

        case QLocale::Portuguese:
            return QStringLiteral( "Português" );

        case QLocale::Romanian:
            return QStringLiteral( "Română" );

        case QLocale::Russian:
            return QStringLiteral( "Русский" );

        case QLocale::Slovenian:
            return QStringLiteral( "Slovenščina" );

        case QLocale::Slovak:
            return QStringLiteral( "Slovenčina" );

        case QLocale::Turkish:
            return QStringLiteral( "Türkçe" );

        case QLocale::Chinese:
            return QStringLiteral( "中文" );

        default:
            return QLocale::languageToString( locale.language() );
    }
}

class InputBox : public QskLinearBox
{
  public:
    InputBox( QQuickItem* parentItem = nullptr )
        : QskLinearBox( Qt::Vertical, parentItem )
    {
        setExtraSpacingAt( Qt::BottomEdge | Qt::RightEdge );

        setMargins( 10 );
        setSpacing( 10 );

        auto* textInput1 = new QskTextInput( this );
        textInput1->setText( "Press and edit Me." );
        textInput1->setSizePolicy( Qt::Horizontal, QskSizePolicy::Preferred );

        auto* textInput2 = new QskTextInput( this );
        textInput2->setText( "Press and edit Me." );
        textInput2->setSizePolicy( Qt::Horizontal, QskSizePolicy::Preferred );
        textInput2->setActivationModes( QskTextInput::ActivationOnAll );

        auto* textInput3 = new QskTextInput( this );
        textInput3->setReadOnly( true );
        textInput3->setText( "Read Only information." );
        textInput3->setSizePolicy( Qt::Horizontal, QskSizePolicy::Preferred );

        auto* textInput4 = new QskTextInput( this );
        textInput4->setEchoMode( QskTextInput::Password );
        textInput4->setPasswordMaskDelay( 1000 );
        textInput4->setMaxLength( 8 );
        textInput4->setText( "12345678" );
        textInput4->setSizePolicy( Qt::Horizontal, QskSizePolicy::Preferred );
    }
};

class LocaleListView final : public QskListView
{
    Q_OBJECT

  public:
    LocaleListView( QQuickItem* parentItem = nullptr )
        : QskListView( parentItem )
        , m_maxWidth( 0.0 )
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

        connect( this, &QskListView::selectedRowChanged,
            [ this ]( int row ) { Q_EMIT selected( localeAt( row ) ); } );
    }

    int rowCount() const override
    {
        return m_values.count();
    }

    int columnCount() const override
    {
        return 1;
    }

    qreal columnWidth( int ) const override
    {
        if ( m_maxWidth == 0.0 )
        {
            const QFontMetricsF fm( effectiveFont( Text ) );

            for ( const auto& entry : m_values )
                m_maxWidth = qMax( m_maxWidth, qskHorizontalAdvance( fm, entry.first ) );

            const auto padding = paddingHint( Cell );
            m_maxWidth += padding.left() + padding.right();
        }

        return m_maxWidth;
    }

    qreal rowHeight() const override
    {
        const QFontMetricsF fm( effectiveFont( Text ) );
        const auto padding = paddingHint( Cell );

        return fm.height() + padding.top() + padding.bottom();
    }

    QVariant valueAt( int row, int ) const override
    {
        return m_values[ row ].first;
    }

    QLocale localeAt( int row ) const
    {
        if ( row >= 0 && row < m_values.size() )
            return m_values[ row ].second;

        return QLocale();
    }

  Q_SIGNALS:
    void selected( const QLocale& );

  private:
    inline void append( const QLocale& locale )
    {
        m_values += qMakePair( nativeLocaleString( locale ), locale );
    }

    QVector< QPair< QString, QLocale > > m_values;
    mutable qreal m_maxWidth;
};

class Window : public QskWindow
{
  public:
    Window()
    {
        populate();
    }

    void populate()
    {
        auto box = new QskLinearBox( Qt::Horizontal );
        box->setSpacing( 10 );
        box->setMargins( 20 );

        auto listView = new LocaleListView( box );
        auto inputBox = new InputBox( box );

        /*
            Disable Qt::ClickFocus, so that the input panel stays open
            when selecting a different locale
         */
        listView->setFocusPolicy( Qt::TabFocus );

        connect( listView, &LocaleListView::selected,
            inputBox, &InputBox::setLocale );

        addItem( box );
        addItem( new QskFocusIndicator() );
    }
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    qputenv( "QT_IM_MODULE", "skinny" );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

#if 1
    // We don't want to have the input panel in a top level window.
    qskDialog->setPolicy( QskDialog::EmbeddedBox );
#endif

    Window window1;
    window1.setObjectName( "Window 1" );
    window1.setColor( "PapayaWhip" );
    window1.resize( 600, 600 );
    window1.show();

#if 1
    Window window2;
    window2.setObjectName( "Window 2" );
    window2.setColor( "Pink" );
    window2.setX( window1.x() + 100 );
    window2.resize( 600, 600 );
    window2.show();
#endif

    return app.exec();
}

#include "main.moc"
