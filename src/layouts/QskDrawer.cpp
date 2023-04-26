#include "QskDrawer.h"
#include "QskAspect.h"
#include "QskControl.h"

#include <QskPopup.h>
#include <QskBox.h>
#include <QskAnimationHint.h>
#include <QskQuick.h>

QSK_SUBCONTROL( QskDrawer, DasPanel )

class QskDrawer::PrivateData {
public:
    QskControl* content;
    QskBox* contentBox;
    Qt::Edge edge = Qt::LeftEdge;
};

QskDrawer::QskDrawer( QQuickItem* parentItem ) :
    Inherited ( parentItem )
    , m_data( new PrivateData { } )
{
    setZ( 1 );

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );

    m_data->contentBox = new QskBox(this);
    m_data->contentBox->setSubcontrolProxy( QskBox::Panel, DasPanel );
    
    setAnimationHint( DasPanel | QskAspect::Position, QskAnimationHint( 1000 ) );

    setFaderAspect( DasPanel | QskAspect::Metric );
    
    setSkinHint( Overlay | QskAspect::Style, false );

    connect(this, &QskDrawer::closed, this, [this](){
	startTransition( DasPanel | QskAspect::Metric,
			 animationHint( DasPanel | QskAspect::Position ),
			 0.0, 1.0 );
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

void QskDrawer::setContent( QskControl* content ) {
    content->setParentItem( m_data->contentBox );
    m_data->content = content;
}

void QskDrawer::updateLayout() {
    auto size = m_data->content->preferredSize();
 
    switch( m_data->edge ) {
    case Qt::Edge::LeftEdge:
	{
	    qreal off = metric( faderAspect() ) * size.width();
	    qskSetItemGeometry( m_data->contentBox,
				-off, 0, size.width(), size.height());
	    break;
	}
    case Qt::Edge::RightEdge:
	{
	    qreal off = metric( faderAspect() ) * size.width();
	    qskSetItemGeometry( m_data->contentBox,
				size.width() + off, 0, size.width(), size.height());
	    break;
	}

    case Qt::Edge::TopEdge:
	{
	    qreal off = metric( faderAspect() ) * size.height();
	    qskSetItemGeometry( m_data->contentBox,
				0, -off, size.width(), size.height());
	    break;
	}

    case Qt::Edge::BottomEdge:
	{
	    qreal off = metric( faderAspect() ) * size.height();
	    qskSetItemGeometry( m_data->contentBox,
				0, size.height() + off, size.width(), size.height());
	    break;
	}
	break;

    }    
   
    Inherited::updateLayout();
}

void QskDrawer::aboutToShow()
{
    startTransition( DasPanel | QskAspect::Metric,
		     animationHint( DasPanel | QskAspect::Position ),
		     1.0, 0.0 );
    Inherited::aboutToShow();
}

#include "moc_QskDrawer.cpp"
