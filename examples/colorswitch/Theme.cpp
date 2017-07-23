/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Theme.h"

#include <QskSetup.h>
#include <QskSkin.h>
#include <QskSkinTransition.h>
#include <QskAspect.h>
#include <QskListView.h>
#include <QskFocusIndicator.h>

static void qskResetColors( QskSkin* skin, const QColor& accent )
{
    skin->resetColors( accent );

    /*
        The current implementation of the skins is not that good
        and we don't have support for customizing them by a minimal set
        of attributes. So we do some manual extra work here
     */
    skin->setColor( QskListView::CellSelected, accent.darker( 130 ) );
    skin->setColor( QskFocusIndicator::Panel | QskAspect::Border, accent.darker( 150 ) );
}

namespace
{
    class SkinTransition : public QskSkinTransition
    {
    public:
        SkinTransition( const QColor& accent ):
            m_accent( accent )
        {
        }

    protected:
        virtual void updateSkin( QskSkin*, QskSkin* newSkin ) override final
        {
            qskResetColors( newSkin, m_accent );
        }

    private:
        const QColor m_accent;
    };
}

Theme::Theme( QObject* parent ):
    QObject( parent ),
    m_accent( qskSetup->skin()->color( QskAspect::Color ) )
{
    connect( qskSetup, &QskSetup::skinChanged,
        [this]( QskSkin* ) { updateColors(); } );

    connect( qskSetup, &QskSetup::skinChanged,
        [this]( QskSkin* ) { Q_EMIT skinChanged(); } );
}

void Theme::setAccent( QColor color )
{
    if ( m_accent != color )
    {
        m_accent = color;
        updateColors();

        Q_EMIT accentChanged();
    }
}

QColor Theme::accent() const
{
    return m_accent;
}

void Theme::setSkin( const QString& skinName )
{
    if ( skinName == qskSetup->skinName() )
        return;

    QskSkin* oldSkin = qskSetup->skin();
    if ( oldSkin->parent() == qskSetup )
        oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

    QskSkin* newSkin = qskSetup->setSkin( skinName );

    SkinTransition transition( m_accent );

    transition.setSourceSkin( oldSkin );
    transition.setTargetSkin( newSkin );
    transition.setAnimation( 500 );

    transition.process();

    if ( oldSkin->parent() == nullptr )
        delete oldSkin;
}

QString Theme::skin() const
{
    return qskSetup->skinName();
}

void Theme::updateColors()
{
    SkinTransition transition( m_accent );

    transition.setMask( SkinTransition::Color );
    transition.setSourceSkin( qskSetup->skin() );
    transition.setTargetSkin( qskSetup->skin() );
    transition.setAnimation( 500 );

    transition.process();
}

#include "moc_Theme.cpp"
