/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMenuButton.h"
#include "QskMenu.h"
#include "QskLabelData.h"

#include <qpointer.h>
#include <qquickwindow.h>

class QskMenuButton::PrivateData
{
  public:
    int triggeredIndex = -1;
    int startIndex = -1;

    QPointer< QskMenu > menu;
    QVector< QskLabelData > options;
};

QskMenuButton::QskMenuButton( QQuickItem* parent )
    : QskMenuButton( QString(), parent )
{
}

QskMenuButton::QskMenuButton( const QString& text, QQuickItem* parent )
    : QskPushButton( text, parent )
    , m_data( new PrivateData )
{
    connect( this, &QskPushButton::pressed,
        this, &QskMenuButton::openMenu );
}

QskMenuButton::~QskMenuButton()
{
}

int QskMenuButton::addOption( const QString& graphicSource, const QString& text )
{
    return addOption( QskLabelData( text, graphicSource ) );
}

int QskMenuButton::addOption( const QUrl& graphicSource, const QString& text )
{
    return addOption( QskLabelData( text, graphicSource ) );
}

int QskMenuButton::addOption( const QskLabelData& option )
{
    const int index = m_data->options.count();
    m_data->options += option;

    if ( m_data->menu )
        m_data->menu->setOptions( m_data->options );

    return index;
}

void QskMenuButton::addSeparator()
{
    addOption( QskLabelData() );
}

void QskMenuButton::setOptions( const QStringList& options )
{
    setOptions( qskCreateLabelData( options ) );
}

void QskMenuButton::setOptions( const QVector< QskLabelData >& options )
{
    m_data->options = options;

    if ( m_data->menu )
        m_data->menu->setOptions( m_data->options );
}

void QskMenuButton::clear()
{
    m_data->options.clear();

    if ( m_data->menu )
        m_data->menu->clear();
}

QVector< QskLabelData > QskMenuButton::options() const
{
    return m_data->options;
}

QskLabelData QskMenuButton::optionAt( int index ) const
{
    return m_data->options.value( index );
}

int QskMenuButton::optionsCount() const
{
    return m_data->options.count();
}

void QskMenuButton::setStartIndex( int index )
{
    m_data->startIndex = index;
}

int QskMenuButton::triggeredIndex() const
{
    return m_data->triggeredIndex;
}

QString QskMenuButton::triggeredText() const
{
    return optionAt( m_data->triggeredIndex ).text();
}

const QskMenu* QskMenuButton::menu() const
{
    return m_data->menu;
}

void QskMenuButton::openMenu()
{
    if ( m_data->menu || window() == nullptr || m_data->options.isEmpty() )
        return;

    m_data->triggeredIndex = -1;

    auto menu = new QskMenu( window()->contentItem() );
    m_data->menu = menu;

    menu->setOptions( m_data->options );
    if ( m_data->startIndex >= 0 )
        menu->setCurrentIndex( m_data->startIndex );

    menu->setOrigin( geometry().bottomLeft() );

    connect( menu, &QskMenu::triggered,
        this, &QskMenuButton::updateTriggeredIndex );

    menu->open();
}

void QskMenuButton::updateTriggeredIndex( int index )
{
    if ( m_data->triggeredIndex != index )
    {
        m_data->triggeredIndex = index;
        Q_EMIT triggered( index );
    }
}

#include "moc_QskMenuButton.cpp"
