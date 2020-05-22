#include "MenuBar.h"

#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskGraphicIO.h>
#include <QskTextLabel.h>

#include <QtGui/QImage>

MenuItem::MenuItem( const QString& name, QQuickItem* parent ) : QskLinearBox( Qt::Horizontal, parent ),
    m_name( name )
{
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );

    QString fileName = ":/images/" + name.toLower() + ".png";
    QImage image( fileName );
    auto graphic = QskGraphic::fromImage( image );
    auto* graphicLabel = new QskGraphicLabel( graphic, this );
    graphicLabel->setFixedSize( 32, 32 );

    auto* textLabel = new QskTextLabel( name, this );
    textLabel->setTextColor( Qt::white ); // ### style
}

MenuBar::MenuBar( QQuickItem *parent ) : QskLinearBox( Qt::Vertical, parent )
{
    setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Preferred );
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );
    setMargins( 35 ); // ### style
    setSpacing( 30 ); // ### style

    setBackgroundColor( { 0, 35, 102 } ); // ### style

    m_entries = { "Home", "Details", "Statistics", "Settings" };

    for( const auto entry : m_entries )
    {
        auto* menuItem = new MenuItem( entry, this );
    }

    addSpacer( 0, 1 ); // fill the space at the bottom
}
