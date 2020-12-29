/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MyToggleButton.h"

#include <QskAspect.h>
#include <QskGraphic.h>
#include <QskGraphicProvider.h>
#include <QskTextOptions.h>

QSK_SUBCONTROL( MyToggleButton, Panel )
QSK_SUBCONTROL( MyToggleButton, Cursor )
QSK_SUBCONTROL( MyToggleButton, CheckedPanel )
QSK_SUBCONTROL( MyToggleButton, CheckedLabel )
QSK_SUBCONTROL( MyToggleButton, UncheckedPanel )
QSK_SUBCONTROL( MyToggleButton, UncheckedLabel )
QSK_SUBCONTROL( MyToggleButton, CheckedIcon )
QSK_SUBCONTROL( MyToggleButton, UncheckedIcon )

class MyToggleButton::PrivateData
{
  public:
    struct
    {
        QString text;

        QString iconSource;
        QskGraphic icon;
        bool iconDirty = false;

    } content[ 2 ];

    QskTextOptions textOptions;

    /*
        Normal: Checked: left, Unchecked : right
        Inverted: Checked: right, Unchecked : left
     */
    bool inverted = false;
};

MyToggleButton::MyToggleButton( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setCheckable( true );
    setAcceptHoverEvents( false );

    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

MyToggleButton::~MyToggleButton()
{
}

void MyToggleButton::setInverted( bool on )
{
    if ( m_data->inverted != on )
    {
        m_data->inverted = on;
        update();

        Q_EMIT invertedChanged( on );
    }
}

bool MyToggleButton::isInverted() const
{
    return m_data->inverted;
}

void MyToggleButton::setTextOptions( const QskTextOptions& options )
{
    if( options != m_data->textOptions )
    {
        m_data->textOptions = options;

        if( !( m_data->content[ 0 ].text.isEmpty() && m_data->content[ 1 ].text.isEmpty() ) )
        {
            update();
        }
    }
}

QskTextOptions MyToggleButton::textOptions() const
{
    return m_data->textOptions;
}

void MyToggleButton::setTextAt( int index, const QString& text )
{
    if( index < 0 || index > 1 )
    {
        return;
    }

    auto& data = m_data->content[ index ];

    if( !data.icon.isNull() )
    {
        // we don't support text + graphic
        data.icon.reset();
        update();
    }

    data.iconDirty = false;

    if( text != data.text )
    {
        data.text = text;
        update();
    }
}

QString MyToggleButton::textAt( int index ) const
{
    if( index < 0 || index > 1 )
    {
        return QString();
    }

    return m_data->content[ index ].text;
}

void MyToggleButton::setIconAt( int index, const QString& icon )
{
    if( index < 0 || index > 1 )
    {
        return;
    }

    auto& data = m_data->content[ index ];

    if( !data.text.isEmpty() )
    {
        // we don't support text + graphic
        data.text.clear();
        update();
    }

    if( icon != data.iconSource )
    {
        data.icon.reset();
        data.iconSource = icon;
        data.iconDirty = true;

        polish();
        update();
    }
}

QString MyToggleButton::iconAt( int index ) const
{
    if( index < 0 || index > 1 )
    {
        return QString();
    }

    return m_data->content[ index ].iconSource;
}

QskGraphic MyToggleButton::graphicAt( int index ) const
{
    if( index < 0 || index > 1 )
    {
        return QskGraphic();
    }

    auto& data = const_cast< MyToggleButton* >( this )->m_data->content[ index ];

    if( data.iconDirty )
    {
        data.icon = Qsk::loadGraphic( data.iconSource );
        data.iconDirty = false;
    }

    return data.icon;
}

void MyToggleButton::updateResources()
{
    for( int i = 0; i < 2; i++ )
    {
        if( m_data->content[ i ].iconDirty )
        {
            ( void )graphicAt( Checked );
        }
    }
}

#include "moc_MyToggleButton.cpp"
