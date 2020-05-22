#include "Card.h"

#include <QskTextLabel.h>

Card::Card(const QString &title, QskControl *content, QQuickItem *parent ) : QskLinearBox( Qt::Vertical, parent ),
    m_title( title )
{
    m_label = new QskTextLabel( m_title, this );
    m_content = content;
    m_content->setParentItem( this );
    m_content->setParent( this );
}
