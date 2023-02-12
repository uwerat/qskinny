#include "QskRadioBox.h"
#include "QskEvent.h"
#include <qnamespace.h>

QSK_SUBCONTROL( QskRadioBox, Panel )
QSK_SUBCONTROL( QskRadioBox, Button )
QSK_SUBCONTROL( QskRadioBox, Symbol )
QSK_SUBCONTROL( QskRadioBox, Text )
QSK_SUBCONTROL( QskRadioBox, Ripple )

QSK_STATE( QskRadioBox, Selected, QskAspect::FirstUserState << 1)
QSK_STATE( QskRadioBox, Pressed, QskAspect::FirstUserState << 2)
QSK_STATE( QskRadioBox, Focused, QskAspect::FirstUserState << 3)

class QskRadioBox::PrivateData {
public:
    QStringList items;
    int selectedIndex = -1;
    int pressedIndex = -1;
    int focusedIndex = -1;
};

QskRadioBox::QskRadioBox( QQuickItem* parent ) :
    Inherited( parent ),
    m_data( new PrivateData{} )
{
    setFocusPolicy( Qt::NoFocus );
    setAcceptedMouseButtons( Qt::LeftButton );

    connect(this, &QskRadioBox::itemsChanged, this,
	    [this]( const QStringList& items ) {
	if( items.count() > 0 ) {
	    setFocusPolicy( Qt::StrongFocus );
	} else {
	    setFocusPolicy( Qt::NoFocus );
	}
    });
}

QskRadioBox::QskRadioBox( const QStringList& list, QQuickItem* parent ) :
    QskRadioBox( parent )
{
    setItems( list );
}

QskRadioBox::QskRadioBox( const QStringList& items,
				  int selectedIndex,
				  QQuickItem* parent ) :
    QskRadioBox( items, parent )
{
    if( selectedIndex >= 0 && selectedIndex < items.count() )
    {
	m_data->selectedIndex = selectedIndex;
    }
}

int QskRadioBox::selectedIndex() const {
    return m_data->selectedIndex;
}

const QStringList& QskRadioBox::items() const {
    return m_data->items;
}

int QskRadioBox::focusedIndex() const {
    return m_data->focusedIndex;
}

int QskRadioBox::pressedIndex() const {
    return m_data->pressedIndex;
}

void QskRadioBox::setSelectedIndex( int index ) {
    if( index == m_data->selectedIndex
    	|| index >= m_data->items.count() ) {
    	return;
    }

    if( index < 0 ) {
	m_data->selectedIndex = -1;
    } else {
	m_data->selectedIndex = index;
    }
   
    selectedIndexChanged( m_data->selectedIndex );
}

void QskRadioBox::setItems( const QStringList& items ){
    if( m_data->items == items ) {
	return;
    }

    m_data->items = items;
    itemsChanged( items );
    setSelectedIndex( m_data->selectedIndex );
}

void QskRadioBox::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
	m_data->selectedIndex = qMax(m_data->selectedIndex - 1, 0);
	m_data->focusedIndex = m_data->selectedIndex;
	setSkinStateFlag( QskRadioBox::Selected );
	event->setAccepted( true );
	update();
	return;
    case Qt::Key_Down:
    case Qt::Key_Right:
	m_data->selectedIndex = qMin(m_data->selectedIndex + 1, items().size() - 1);
	m_data->focusedIndex = m_data->selectedIndex;
	setSkinStateFlag( QskRadioBox::Selected );
	event->setAccepted( true );
	update();
	return;
    case Qt::Key_Select:
    case Qt::Key_Return:
    case Qt::Key_Space:
	m_data->selectedIndex = m_data->focusedIndex;
	setSkinStateFlag( QskRadioBox::Selected );
	setSkinStateFlag( QskRadioBox::Pressed );
	event->setAccepted( true );
	update();
	return;
    }

    auto nextTabIndex = m_data->focusedIndex;
    nextTabIndex += qskFocusChainIncrement( event );
    if( nextTabIndex >= items().size()
	|| nextTabIndex < 0 ) {	
	Inherited::keyPressEvent( event );	
    } else {
	m_data->focusedIndex = nextTabIndex;
	setSkinStateFlag( QskRadioBox::Focused );
	event->setAccepted( true );
	update();
    }
}

void QskRadioBox::keyReleaseEvent( QKeyEvent* e )
{
    setSkinStateFlag( QskRadioBox::Pressed, false );
    e->setAccepted( true );
    update();
}

void QskRadioBox::mousePressEvent( QMouseEvent* e )
{
    auto indexAtPosition = indexAt(e->localPos());

    m_data->pressedIndex = indexAtPosition;
    m_data->selectedIndex = -1;

    m_data->focusedIndex = indexAtPosition;

    setSkinStateFlag( QskRadioBox::Pressed );

    e->setAccepted( true );
    update();    
}

void QskRadioBox::mouseReleaseEvent( QMouseEvent* e )
{
    setSkinStateFlag( QskRadioBox::Pressed, false );
    
    auto index = indexAt( e->localPos() );
    if( index == m_data->pressedIndex ) {    
	setSelectedIndex( index );
    }

    e->setAccepted( true );
    update();
}

void QskRadioBox::focusInEvent( QFocusEvent* e ) {
    if( e->reason() == Qt::TabFocusReason ) {
	m_data->focusedIndex = 0;
    } else if( e->reason() == Qt::BacktabFocusReason ) {
	m_data->focusedIndex = items().size() - 1;
    }

    setSkinStateFlag( Focused );
    Inherited::focusInEvent( e );
}

void QskRadioBox::focusOutEvent( QFocusEvent* e ) {
    m_data->focusedIndex = -1;
    setSkinStateFlag( Focused, false );
    update();
    Inherited::focusOutEvent( e );
}

int QskRadioBox::indexAt( const QPointF& target ) const {
    auto itemHeight = contentsRect().height() / items().size();
    auto index = target.y() / itemHeight;

    if( index < 0 || index >= items().size() )
	return -1;

    return index;
}

#include "moc_QskRadioBox.cpp"
