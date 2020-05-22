#include "MenuBar.h"

#include <QskTextLabel.h>

MenuBar::MenuBar( QQuickItem *parent ) : QskLinearBox( Qt::Vertical, parent )
{
    setSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Expanding );
    setAutoLayoutChildren( true );
    setAutoAddChildren( true );

    setBackgroundColor( { 0, 35, 102 } ); // ### style

    m_entries = { "Home", "Details", "Statistics", "Usage" };

    for( const auto entry : m_entries )
    {
        auto* label = new QskTextLabel( entry, this );
        label->setTextColor( Qt::white ); // ### style
    }
}
