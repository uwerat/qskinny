#include "QskCheckBox.h"

#include "QskAspect.h"

#include <qset.h>

QSK_SUBCONTROL( QskCheckBox, Box )
QSK_SUBCONTROL( QskCheckBox, Tick )

QSK_SYSTEM_STATE( QskCheckBox, PartiallyChecked, QskAspect::LastUserState << 2 )

struct QskCheckBox::PrivateData
{
    Qt::CheckState checkState;
    bool checkStateChanging : 1;
    bool toggleChanging : 1;
    bool triState : 1;
};

QskCheckBox::QskCheckBox( QQuickItem* parent )
    : Inherited( parent ), m_data( new PrivateData{ Qt::CheckState::Unchecked,
        false, false, false } ) {
    setAcceptHoverEvents( true );
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    connect( this, &QskCheckBox::checkedChanged, this, [ this ]( bool t )
    {
        setCheckStateInternal( t ? Qt::CheckState::Checked :
        Qt::CheckState::Unchecked );
    } );
}

QskCheckBox::~QskCheckBox()
{
}

bool QskCheckBox::isCheckable() const
{
    return true;
}

Qt::CheckState QskCheckBox::checkState() const
{
    return m_data->checkState;
}

void QskCheckBox::setCheckStateInternal( Qt::CheckState checkState )
{
    if( m_data->checkStateChanging )
    {
        return;
    }

    setSkinStateFlag( PartiallyChecked,
        checkState == Qt::CheckState::PartiallyChecked );

    m_data->checkState = checkState;
    Q_EMIT checkStateChanged( checkState );
}

void QskCheckBox::setCheckState( Qt::CheckState checkState )
{
    if( checkState == m_data->checkState )
        return;

    m_data->checkStateChanging = true;
    if( checkState == Qt::CheckState::PartiallyChecked )
    {
        setChecked( true );
        setTriState( true );
    }
    else
    {
        setChecked( checkState == Qt::CheckState::Checked );
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

#include "moc_QskCheckBox.cpp"
