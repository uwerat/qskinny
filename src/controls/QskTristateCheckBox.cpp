#include "QskTristateCheckBox.h"
#include "QskAspect.h"

#include <qset.h>

QSK_SYSTEM_STATE( QskTristateCheckBox, PartiallyChecked, QskAspect::LastUserState << 2 )

class QskTristateCheckBox::PrivateData
{
  public:
    PrivateData()
        : checkState( Qt::Unchecked )
        , checkStateChanging( false )
    {
    }
    Qt::CheckState checkState : 2;
    bool checkStateChanging : 1;
};

QskTristateCheckBox::QskTristateCheckBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setAcceptHoverEvents( true );
    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

    connect( this, &QskAbstractButton::checkedChanged, this,
        [ this ]( bool on ) { setCheckStateInternal( on ? Qt::Checked : Qt::Unchecked ); } );
}

QskTristateCheckBox::~QskTristateCheckBox()
{
}

Qt::CheckState QskTristateCheckBox::checkState() const
{
    return m_data->checkState;
}

void QskTristateCheckBox::setCheckStateInternal( Qt::CheckState checkState )
{
    if( m_data->checkStateChanging )
        return;

    setSkinStateFlag( PartiallyChecked, checkState == Qt::PartiallyChecked );

    m_data->checkState = checkState;
    Q_EMIT checkStateChanged( checkState );
}

void QskTristateCheckBox::setCheckState( Qt::CheckState checkState )
{
    if( checkState == m_data->checkState )
        return;

    m_data->checkStateChanging = true;

    setChecked( checkState == Qt::PartiallyChecked
        || checkState == Qt::Checked );

    m_data->checkStateChanging = false;

    setCheckStateInternal( checkState );
}

#include "moc_QskTristateCheckBox.cpp"
