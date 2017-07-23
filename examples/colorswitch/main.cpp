/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include <SkinnyFont.h>
#include <SkinnyShortcut.h>

#include <QskModule.h>
#include <QskSetup.h>
#include <QskSkin.h>
#include <QskSkinTransition.h>
#include <QskAspect.h>
#include <QskObjectCounter.h>

#include <QskFocusIndicator.h>
#include <QskListView.h>
#include <QskSlider.h>

#include <QQmlApplicationEngine>
#include <QGuiApplication>

class SkinTransition: public QskSkinTransition
{
public:
    SkinTransition( const QColor& accent ):
        m_accent( accent )
    {
    }

protected:
    virtual void updateSkin( QskSkin*, QskSkin* skin ) override final
    {
        skin->resetColors( m_accent );
        skin->setColor( QskListView::CellSelected, m_accent.darker( 130 ) );
        skin->setColor( QskFocusIndicator::Panel | QskAspect::Border, m_accent.darker( 150 ) );
    }

private:
    const QColor m_accent;
};

class Theme: public QObject
{
    Q_OBJECT
    Q_PROPERTY( QColor accent READ accent WRITE setAccent NOTIFY accentChanged )

public:
    Theme( QObject* parent = nullptr ):
        QObject( parent ),
        m_accent( qskSetup->skin()->color( QskAspect::Color ) )
    {
        connect( qskSetup, &QskSetup::skinChanged,
            [this]( QskSkin* ) { updateColors(); } );
    }

    void setAccent( QColor color )
    {
        if ( m_accent != color )
        {
            m_accent = color;
            updateColors();

            Q_EMIT accentChanged();
        }
    }

    QColor accent() const
    {
        return m_accent;
    }

Q_SIGNALS:
    void accentChanged();

private:
    void updateColors()
    {
        SkinTransition transition( m_accent );

        transition.setMask( SkinTransition::Color );
        transition.setSourceSkin( qskSetup->skin() );
        transition.setTargetSkin( qskSetup->skin() );
        transition.setAnimation( 500 );

        transition.process();
    }

    QColor m_accent;
};

int main( int argc, char* argv[] )
{
#ifdef ITEM_STATISTICS
    QskObjectCounter counter( true );
#endif

    qputenv( "QT_IM_MODULE", "skinny" );

    QskModule::registerTypes();
    qmlRegisterType< Theme >( "Theme", 1, 0, "Theme" );

    QGuiApplication app( argc, argv );

    SkinnyFont::init( &app );
    SkinnyShortcut::enable( SkinnyShortcut::Quit |
        SkinnyShortcut::DebugShortcuts );

    QQmlApplicationEngine engine( QUrl( "qrc:/qml/colorswitch.qml" ) );

    return app.exec();
}

#include "main.moc"
