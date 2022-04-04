/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCheckBox.h"
#include "QskAspect.h"

#include <qset.h>

QSK_SUBCONTROL( QskCheckBox, Box )
QSK_SUBCONTROL( QskCheckBox, Tick )

QSK_SYSTEM_STATE( QskCheckBox, PartiallyChecked, QskAspect::LastUserState << 2 )

class QskCheckBox::PrivateData
{
  public:
    PrivateData()
        : checkState( Qt::Unchecked )
        , checkStateChanging( false )
        , toggleChanging( false )
        , triState( false )
    {
    }

    QSet< QskAbstractButton* > group;

    int groupItemsChecked = 0;

    int checkState : 2;
    bool checkStateChanging : 1;
    bool toggleChanging : 1;
    bool triState : 1;
};

QskCheckBox::QskCheckBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setAcceptHoverEvents( true );
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    connect( this, &QskCheckBox::checkedChanged, this,
        [ this ]( bool on ) { setCheckStateInternal( on ? Qt::Checked : Qt::Unchecked ); } );
}

QskCheckBox::~QskCheckBox()
{
    Q_EMIT removeFromAllGroupsRequested();
}

bool QskCheckBox::isCheckable() const
{
    return true;
}

Qt::CheckState QskCheckBox::checkState() const
{
    return static_cast< Qt::CheckState >( m_data->checkState );
}

void QskCheckBox::setCheckStateInternal( Qt::CheckState checkState )
{
    if( m_data->checkStateChanging )
        return;

    setSkinStateFlag( PartiallyChecked, checkState == Qt::PartiallyChecked );

    m_data->checkState = checkState;
    Q_EMIT checkStateChanged( checkState );
}

void QskCheckBox::setCheckState( Qt::CheckState checkState )
{
    if( checkState == m_data->checkState )
        return;

    m_data->checkStateChanging = true;

    if( checkState == Qt::PartiallyChecked )
    {
        setChecked( true );
        setTriState( true );
    }
    else
    {
        setChecked( checkState == Qt::Checked );
    }

    m_data->checkStateChanging = false;

    setCheckStateInternal( checkState );
}

bool QskCheckBox::isTriState() const
{
    return m_data->triState;
}

void QskCheckBox::setTriState( bool triState )
{
    if( m_data->triState != triState )
    {
        m_data->triState = triState;
        Q_EMIT isTriStateChanged( triState );
    }
}

void QskCheckBox::updated()
{
    if( m_data->toggleChanging )
        return;

    const auto& groupItemsChecked = m_data->groupItemsChecked;

    if( groupItemsChecked == m_data->group.size() )
    {
        setCheckState( Qt::Checked );
    }
    else if ( groupItemsChecked == 0 )
    {
        setCheckState( Qt::Unchecked );
    }
    else
    {
        setCheckState( Qt::PartiallyChecked );
    }
}

void QskCheckBox::addToGroup( QskCheckBox* groupItem )
{
    if( m_data->group.contains( groupItem ) )
        return;

    m_data->group.insert( groupItem );

    if( groupItem->checkState() == Qt::Checked )
        m_data->groupItemsChecked++;

    updated();

    connect( this, &QskCheckBox::checkStateChanged,
        groupItem, [ this, groupItem ]( Qt::CheckState checkState )
    {
        if( checkState == Qt::Checked )
        {
            m_data->toggleChanging = true;
            groupItem->setChecked( true );
            m_data->groupItemsChecked = m_data->group.size();
            m_data->toggleChanging = false;
        }
        else if ( checkState == Qt::Unchecked )
        {
            m_data->toggleChanging = true;
            groupItem->setChecked( false );
            m_data->groupItemsChecked = 0;
            m_data->toggleChanging = false;
        }
    } );

    connect( groupItem, &QskAbstractButton::toggled,
        this, [ this, groupItem ]( bool toggled )
    {
        auto& groupItemsChecked = m_data->groupItemsChecked;
        groupItemsChecked += toggled ? 1 : -1;
        updated();
    } );

    connect( groupItem, &QskCheckBox::removeFromAllGroupsRequested,
        this, [ this, groupItem ]( ) { removeFromGroup( groupItem ); } );
}

void QskCheckBox::removeFromGroup( QskCheckBox* groupItem )
{
    if( !m_data->group.remove( groupItem ) )
        return;

    if( groupItem->checkState() == Qt::Checked )
        m_data->groupItemsChecked--;

    updated();
}

#include "moc_QskCheckBox.cpp"
