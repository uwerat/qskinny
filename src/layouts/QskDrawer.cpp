#include "QskDrawer.h"
#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskControl.h"
#include "QskPopup.h"
#include "QskBox.h"
#include <algorithm>
#include <qnamespace.h>
#include <qquickitem.h>
#include <QskQuick.h>

#include <QskSkinlet.h>
#include <QskSkin.h>
#include <QskWindow.h>
#include <qsize.h>

QSK_SUBCONTROL( QskDrawer, DasPanel )

class QskDrawer::PrivateData {
public:
    QskControl* control = nullptr;
    QskBox* content;
    Qt::Edge edge = Qt::LeftEdge;
};

QskDrawer::QskDrawer( QQuickItem* parentItem ) :
    Inherited ( parentItem )
    , m_data( new PrivateData { } )
{
    using Q = QskDrawer;
    setZ( 1 );

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );

    m_data->content = new QskBox(this);
    m_data->content->setSubcontrolProxy( QskBox::Panel, QskDrawer::DasPanel );
    
    setAnimationHint( QskDrawer::DasPanel | QskAspect::Metric, QskAnimationHint(300) );

    setFaderAspect( DasPanel | QskAspect::Metric );
    
    setSkinHint( Q::Overlay | QskAspect::Style, false );

    connect(this, &QskDrawer::closed, this, [this](){
	startTransition( DasPanel | QskAspect::Metric,
			 QskAnimationHint(200), 0.0, 1.0 );
    });
}

QskDrawer::~QskDrawer()
{
}

Qt::Edge QskDrawer::edge() const { 
    return m_data->edge;
}

void QskDrawer::setEdge( Qt::Edge edge ) {
    if( m_data->edge == edge ) {
	return;
    }

    m_data->edge = edge;
    edgeChanged( edge );
}

void QskDrawer::setContent( QskControl* t ) {
    m_data->control = t;
    t->setParentItem( m_data->content );
}

void QskDrawer::updateLayout() {
    if( !m_data->control ) {
	return;
    }

    auto rect = parentItem()->childrenRect();

    switch( m_data->edge ) {
    case Qt::Edge::LeftEdge:
    case Qt::Edge::RightEdge:
	//	rect.setWidth( m_data->control->preferredSize().width() );
	break;

    case Qt::Edge::TopEdge:
    case Qt::Edge::BottomEdge:
	//	rect.setHeight( m_data->control->preferredSize().height() );
	break;

    }
    
    auto size = rect.size();
    qreal off = metric( faderAspect() ) * size.width();

    qskSetItemGeometry( m_data->content, -off, 0, size.width(), size.height());

    Inherited::updateLayout();
}

void QskDrawer::aboutToShow()
{
    startTransition( DasPanel | QskAspect::Metric, QskAnimationHint(200), 1.0, .0 );
    Inherited::aboutToShow();
}


#include "moc_QskDrawer.cpp"
