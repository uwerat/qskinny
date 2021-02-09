#include "SkinnyShortcut.h"

#include <QskShortcutMap.h>
#include <QskSetup.h>
#include <QskSkinManager.h>
#include <QskWindow.h>
#include <QskAspect.h>
#include <QskSkin.h>
#include <QskControl.h>
#include <QskQuick.h>
#include <QskSkinTransition.h>

#include <QQuickItem>
#include <QKeySequence>
#include <QGuiApplication>
#include <QSGNode>
#include <QDebug>

#include <unordered_map>
#include <iostream>

SkinnyShortcut::SkinnyShortcut( QObject* parent ):
    QObject( parent )
{
}

void SkinnyShortcut::enable( Types types )
{
    using namespace std;

    static SkinnyShortcut s_shortcut;

    if ( types & RotateSkin )
    {
        QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL | Qt::Key_S ),
            false, &s_shortcut, &SkinnyShortcut::rotateSkin );
        cout << "CTRL-S to change the skin." << endl;
    }

    if ( types & ChangeFonts )
    {
        QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL | Qt::Key_F ),
            false, &s_shortcut, [] { s_shortcut.changeFonts( +1 ); } );

        QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL | Qt::Key_G ),
            false, &s_shortcut, [] { s_shortcut.changeFonts( -1 ); } );

        cout << "CTRL-F to increase the font size." << endl;
        cout << "CTRL-G to decrease the font size." << endl;
    }

    if ( types & DebugBackground )
    {
        QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL | Qt::Key_B ),
            false, &s_shortcut, &SkinnyShortcut::showBackground );
        cout << "CTRL-B to enable visual debugging modes." << endl;
    }

    if ( types & DebugStatistics )
    {
        QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL | Qt::Key_K ),
            false, &s_shortcut, &SkinnyShortcut::debugStatistics );
        cout << "CTRL-K to dump statistics about the items/nodes being currently used." << endl;
    }

    if ( types & Quit )
    {
        // QKeySequence::Quit crashes the application
        // when not being implemented by the platform !!

        QskShortcutMap::addShortcut( QKeySequence( Qt::CTRL | Qt::Key_Q ),
            false, QGuiApplication::instance(), &QGuiApplication::quit );
        cout << "CTRL-Q to terminate the application." << endl;
    }
}

void SkinnyShortcut::rotateSkin()
{
    const QStringList names = qskSkinManager->skinNames();
    if ( names.size() <= 1 )
        return;

    int index = names.indexOf( qskSetup->skinName() );
    index = ( index + 1 ) % names.size();

    QskSkin* oldSkin = qskSetup->skin();
    if ( oldSkin->parent() == qskSetup )
        oldSkin->setParent( nullptr ); // otherwise setSkin deletes it

    QskSkin* newSkin = qskSetup->setSkin( names[ index ] );

    QskSkinTransition transition;

    //transition.setMask( QskAspect::Color ); // Metrics are flickering -> TODO
    transition.setSourceSkin( oldSkin );
    transition.setTargetSkin( newSkin );
    transition.setAnimation( 500 );

    transition.process();

    if ( oldSkin->parent() == nullptr )
        delete oldSkin;
}

void SkinnyShortcut::showBackground()
{
#if 0
    const QVector< QByteArray > sgDebugModes =
        { "clip", "overdraw", "changes", "batches" };
#else
    const QVector< QByteArray > sgDebugModes = { "overdraw" };
#endif

    static int id = 0;

    id = ( id + 1 ) % ( sgDebugModes.count() + 2 );

    bool forceBackground = false;
    QByteArray scengraphDebugMode;

    if ( id == 0 )
    {
        // nothing
    }
    else if ( id == 1 )
    {
        forceBackground = true;
    }
    else
    {
        scengraphDebugMode = sgDebugModes[ id - 2 ];
    }

    qskSetup->setItemUpdateFlag( QskQuickItem::DebugForceBackground, forceBackground );

    const auto windows = QGuiApplication::topLevelWindows();
    for ( auto window : windows )
    {
        if ( QskWindow* w = qobject_cast< QskWindow* >( window ) )
        {
            if ( w->customRenderMode() != scengraphDebugMode )
            {
                w->setCustomRenderMode( scengraphDebugMode );
                w->update();
            }
        }
    }
}

void SkinnyShortcut::changeFonts( int increment )
{
    auto skin = qskSetup->skin();

    const auto fonts = skin->fonts();

    for ( auto it = fonts.begin(); it != fonts.end(); ++it )
    {
        auto role = it->first;
        auto font = it->second;

        if ( font.pixelSize() > 0 )
        {
            const auto newSize = font.pixelSize() + increment;
            if ( newSize > 0 )
                font.setPixelSize( newSize );
        }
        else
        {
            const auto newSize = font.pointSizeF() + increment;
            if ( newSize > 0 )
                font.setPointSizeF( font.pointSizeF() + increment );
        }

        skin->setFont( role, font );
    }

    Q_EMIT qskSetup->skinChanged( skin );
}

static inline void countNodes( const QSGNode* node, int& counter )
{
    if ( node )
    {
        counter++;

        for ( auto child = node->firstChild();
            child != nullptr; child = child->nextSibling() )
        {
            countNodes( child, counter );
        }
    }
}

static void countNodes( const QQuickItem* item, int& counter )
{
    const auto itemNode = qskItemNode( item );

    if ( itemNode )
    {
        auto node = qskPaintNode( item );
        if ( node )
        {
            countNodes( node, counter );
            while ( node != itemNode )
            {
                counter++;
                node = node->parent();
            }
        }
        else
        {
            counter++;
            // clipNode/opacityNode ???
        }
    }
}

static void countItems( const QQuickItem* item, int counter[4] )
{
    if ( item )
    {
        counter[0]++;

        int nodeCounter = 0;
        countNodes( item, nodeCounter );

        counter[2] += nodeCounter;

        if ( item->isVisible() )
        {
            counter[1]++;
            counter[3] += nodeCounter;
        }

        const auto children = item->childItems();
        for ( const auto* child : children )
            countItems( child, counter );
    }
}

void SkinnyShortcut::debugStatistics()
{
    const auto windows = QGuiApplication::topLevelWindows();
    for ( auto window : windows )
    {
        const auto w = qobject_cast< const QQuickWindow* >( window );
        if ( w == nullptr )
            continue;

        int counter[4] = {};
        countItems( w->contentItem(), counter );

        qDebug() << w << "\n\titems:" << counter[0] << "visible" << counter[1]
            << "\n\tnodes:" << counter[2] << "visible" << counter[3]; 
    }
}

#include "moc_SkinnyShortcut.cpp"
