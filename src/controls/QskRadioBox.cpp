#include "QskRadioBox.h"
#include "QskEvent.h"
#include "QskAnimationHint.h"
#include "QskSkinlet.h"

#include <qglobal.h>
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
    int focusedIndex = -1;
    int pressedIndex = -1;
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

    setFocusedIndex( -1 );
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

QRectF QskRadioBox::focusIndicatorRect() const {
    if( m_data->focusedIndex > -1) {

	auto textRect = effectiveSkinlet()->sampleRect( this,
	    contentsRect(), QskRadioBox::Text, m_data->focusedIndex );

	auto buttonRect = effectiveSkinlet()->sampleRect( this,
	       contentsRect(), QskRadioBox::Button, m_data->focusedIndex );

	auto result = QRectF(
	   qMin( textRect.x(), buttonRect.x() ),
           qMin( textRect.y(), buttonRect.y() ),
           buttonRect.width() + textRect.width(),
	   qMax( buttonRect.height(), textRect.height() ));

	if( layoutMirroring() ) {
	    result.setWidth(
		result.width() + marginHint( Text ).right()
		+ marginHint( Button ).left() );
	} else {
	    result.setWidth(
		result.width() + marginHint( Text ).left()
		+ marginHint( Button ).right() );
	}

	return result;
    }

    return QRectF();
}

int QskRadioBox::selectedIndex() const {
    return m_data->selectedIndex;
}

const QStringList& QskRadioBox::items() const {
    return m_data->items;
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

    if( m_data->focusedIndex > items.size() ) {
	setFocusedIndex( 0 );
    }
}

void QskRadioBox::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
	m_data->selectedIndex = qMax(m_data->selectedIndex - 1, 0);
	setFocusedIndex( m_data->selectedIndex );
	event->setAccepted( true );
	update();
	return;
    case Qt::Key_Down:
    case Qt::Key_Right:
	m_data->selectedIndex = qMin(m_data->selectedIndex + 1, items().size() - 1);
	setFocusedIndex( m_data->selectedIndex );
	event->setAccepted( true );
	update();
	return;
    case Qt::Key_Select:
    case Qt::Key_Return:
    case Qt::Key_Space:
	m_data->selectedIndex = m_data->focusedIndex;
	event->setAccepted( true );
	update();
	return;
    }

    auto currentTabIndex = m_data->focusedIndex;
    auto nextTabIndex = currentTabIndex + qskFocusChainIncrement( event );
    if( nextTabIndex >= items().size()
	|| nextTabIndex < 0 ) {	
	Inherited::keyPressEvent( event );
	setFocusedIndex( -1);	
	update();
    } else {
	event->setAccepted( true );
        setFocusedIndex( (float) nextTabIndex );

	const auto aspect = Ripple | QskAspect::Metric | QskAspect::Position;
	auto hint = animationHint(aspect | skinStates());
	if( hint.isValid()) {
	    startTransition( aspect,
			     hint,
			     (float) currentTabIndex, (float) nextTabIndex );
	}

	update();

    }
}

void QskRadioBox::keyReleaseEvent( QKeyEvent* e )
{
    e->setAccepted( true );
}

void QskRadioBox::mousePressEvent( QMouseEvent* e )
{
    auto indexAtPosition = indexAt(e->localPos());

    m_data->pressedIndex = indexAtPosition;
    m_data->selectedIndex = -1;

    setFocusedIndex( indexAtPosition );

    e->setAccepted( true );
    update();
}

void QskRadioBox::mouseReleaseEvent( QMouseEvent* e )
{
    auto index = indexAt( e->localPos() );
    if( index == m_data->pressedIndex ) {    
	setSelectedIndex( index );
    }

    e->setAccepted( true );
    update();
}

void QskRadioBox::focusInEvent( QFocusEvent* e ) {
    if( e->reason() == Qt::TabFocusReason ) {
	setFocusedIndex(0 );
    } else if( e->reason() == Qt::BacktabFocusReason ) {
	setFocusedIndex(items().size() - 1 );
    }

    update();
    Inherited::focusInEvent( e );
}

void QskRadioBox::focusOutEvent( QFocusEvent* e ) {
    setFocusedIndex( -1 );
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

void QskRadioBox::setFocusedIndex( int index ) {
    m_data->focusedIndex = index;
    setPositionHint( Ripple, index );
    focusIndicatorRectChanged();
}

#include "moc_QskRadioBox.cpp"
