#include "MenuBar.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGraphicIO.h>
#include <QskSkin.h>
#include <QskTextLabel.h>

#include <QtGui/QImage>

MenuItem::MenuItem( const QString& name, QQuickItem* parent ) : QskLinearBox( Qt::Horizontal, parent ),
    m_name( name )
{
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );
    setPreferredWidth( 140 );

    QString fileName = ":/images/" + name.toLower() + ".png";
    QImage image( fileName );
    auto graphic = QskGraphic::fromImage( image );
    auto* graphicLabel = new QskGraphicLabel( graphic, this );
    graphicLabel->setSizePolicy(QskSizePolicy::Fixed, QskSizePolicy::Fixed);
//    graphicLabel->setFixedSize( 32, 32 );

    auto* textLabel = new QskTextLabel( name, this );
    textLabel->setTextColor( Qt::white ); // ### style
    textLabel->setFontRole(QskSkin::SmallFont);
}

MenuBar::MenuBar( QQuickItem *parent ) : QskLinearBox( Qt::Vertical, parent )
{
    setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Preferred );
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );

    setBackgroundColor( "#6D7BFB" ); // ### style

    auto* mainIcon = ":/images/main-icon.png";
    QImage image(mainIcon);
    auto graphic = QskGraphic::fromImage( image );
    auto* graphicLabel = new QskGraphicLabel( graphic, this );
    graphicLabel->setMargins( { 50, 35, 50, 50 });


    m_entries = { "Dashboard", "Rooms", "Devices", "Statistics", "Storage", "Members" };

    for( const auto entry : m_entries )
    {
        auto* menuItem = new MenuItem( entry, this );
    }

    addSpacer( 0, 1 ); // fill the space at the bottom
}
