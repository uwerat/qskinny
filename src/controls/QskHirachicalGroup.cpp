#include "QskHirachicalGroup.h"

#include "QskTristateCheckBox.h"
#include "QskCheckBox.h"

#include <qset.h>

#include <algorithm>

class QskHirachicalGroup::PrivateData
{
  public:
    QskTristateCheckBox* head;
    int checked = 0;
    QSet< QskCheckBox* > group;
    bool headChanging = false;
};

QskHirachicalGroup::QskHirachicalGroup( QObject* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
}

QskHirachicalGroup::QskHirachicalGroup( QskTristateCheckBox* head )
    : Inherited( head )
    , m_data( new PrivateData() )
{
    setHead(head);
}

QskHirachicalGroup::~QskHirachicalGroup()
{
}

void QskHirachicalGroup::setHead( QskTristateCheckBox* head )
{
    if( m_data->head == head )
    {
        return;
    }

    m_data->head = head;

    if( head == nullptr )
    {
        return;
    }

    connect( head, &QskTristateCheckBox::checkStateChanged, head,
        [ this ]( Qt::CheckState checkedState )
    {
        if( checkedState == Qt::PartiallyChecked )
        {
            return;
        }

        m_data->headChanging = true;
        for(auto child : qAsConst(m_data->group) )
        {
            child->setChecked(checkedState == Qt::Checked);
        }
        m_data->headChanging = false;

        if(checkedState == Qt::Checked)
        {
            m_data->checked = m_data->group.size();
        }
        else
        {
            m_data->checked = 0;
        }
    });

    Q_EMIT headChanged( head );
}

QskTristateCheckBox* QskHirachicalGroup::head() const
{
    return m_data->head;
}

void QskHirachicalGroup::add( QskCheckBox* newMember )
{
    if( m_data->group.contains( newMember ) )
    {
        return;
    }

    connect( newMember, &QObject::destroyed, this, [this, newMember ]( QObject* )
    {
        remove( newMember );
    });

    if( m_data->head != nullptr )
    {
        connect( newMember, &QskAbstractButton::toggled,
            m_data->head, [ this ]( bool toggled )
        {
            if( m_data->headChanging == true )
            {
                return;
            }

            if( toggled )
            {
                ++m_data->checked;
            }
            else
            {
                --m_data->checked;
            }

            update();
        });
    }

    m_data->group.insert( newMember );

    Q_EMIT added( newMember );
}

bool QskHirachicalGroup::remove( QskCheckBox* removal )
{
    if( !m_data->group.contains( removal ) )
    {
        return false;
    }

    m_data->group.remove( removal );

    // We need to count again, since if removal is about to be deletet its m_data is
    // already deleted thus we can not check its CheckState.
    m_data->checked = std::count_if( m_data->group.begin(), m_data->group.end(),
        []( QskCheckBox* box ) { return box->isChecked(); } );
    update();

    Q_EMIT removed( removal );
    return true;
}

void QskHirachicalGroup::update() {
    if( m_data->head == nullptr )
    {
        return;
    }

    if( m_data->group.size() == m_data->checked  )
    {
        m_data->head->setCheckState( Qt::Checked );
        return;
    }

    if( m_data->checked == 0 )
    {
        m_data->head->setCheckState( Qt::Unchecked );
        return;
    }

    m_data->head->setCheckState( Qt::CheckState::PartiallyChecked );
}
#include "moc_QskHirachicalGroup.cpp"
