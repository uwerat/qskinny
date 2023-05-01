#include "QskDrawer.h"
#include "QskAspect.h"
#include "QskControl.h"

#include <QskPopup.h>
#include <QskBox.h>
#include <QskAnimationHint.h>
#include <QskQuick.h>

QSK_SUBCONTROL( QskDrawer, Panel )
QSK_SUBCONTROL( QskDrawer, Overlay )

class QskDrawer::PrivateData {
public:
    QskControl* content;
    QskBox* contentBox;
    Qt::Edge edge = Qt::LeftEdge;
    Qt::Alignment alignment = Qt::AlignCenter;
};

QskDrawer::QskDrawer( QQuickItem* parentItem ) :
    Inherited ( parentItem )
    , m_data( new PrivateData { } )
{
    setZ( 1 );

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );

    m_data->contentBox = new QskBox(this);
    m_data->contentBox->setSubcontrolProxy( QskBox::Panel, Panel );
    setSubcontrolProxy( Inherited::Overlay, Overlay );

    setFaderAspect( Panel | QskAspect::Metric );

    connect(this, &QskDrawer::closed, this, [this]() {
	startTransition( Panel | QskAspect::Metric,
			 animationHint( Panel | QskAspect::Position ),
			 0.0, 1.0 );
    });
}

QskDrawer::~QskDrawer()
{
}

Qt::Edge QskDrawer::edge() const {
    return m_data->edge;
}

Qt::Alignment QskDrawer::alignment() const {
    return m_data->alignment;
}

void QskDrawer::setEdge( Qt::Edge edge ) {
    if( m_data->edge == edge ) {
	return;
    }

    update();
    m_data->edge = edge;
    edgeChanged( edge );
}

void QskDrawer::setAlignment( Qt::Alignment alignment ) {
    if( m_data->alignment == alignment ) {
	return;
    }

    update();
    m_data->alignment = alignment;
    alignmentChanged( alignment );
}

void QskDrawer::setContent( QskControl* content ) {
    content->setParentItem( m_data->contentBox );
    m_data->content = content;
}

void QskDrawer::updateLayout() {
    const auto& contentSize = m_data->content->preferredSize();
    const auto& parentSize = parentItem()->size();

    switch( m_data->edge ) {
    case Qt::Edge::LeftEdge:
	{
	    qreal x = metric( faderAspect() ) * contentSize.width() * -1.0;
	    qreal y = 0;

	    if( alignment().testFlag( Qt::AlignVCenter ) ) {
		y = ( parentSize.height() - contentSize.height() ) / 2.0;
	    } else if ( alignment().testFlag( Qt::AlignBottom ) ) {
		y = ( parentSize.height() - contentSize.height() );
	    }

	    qskSetItemGeometry( m_data->contentBox,
				x, y,
				contentSize.width(), parentSize.height() );
	    break;
	}
    case Qt::Edge::RightEdge:
	{
	    qreal x = ( metric( faderAspect() ) * contentSize.width() )
		+ parentSize.width()
		- contentSize.width();
	    qreal y = 0;

	    if( alignment().testFlag( Qt::AlignVCenter ) ) {
		y = ( parentSize.height() - contentSize.height() ) / 2.0;
	    } else if ( alignment().testFlag( Qt::AlignBottom ) ) {
		y = ( parentSize.height() - contentSize.height() );
	    }

	    qskSetItemGeometry( m_data->contentBox,
				x, y,
				contentSize.width(),
				parentSize.height() );
	    break;
	}

    case Qt::Edge::TopEdge:
	{
	    qreal x = 0;
	    qreal y = metric( faderAspect() ) * contentSize.height();

	    if( alignment().testFlag( Qt::AlignCenter) ) {
		x = ( parentSize.width() - contentSize.width() ) / 2;
	    } else if( alignment().testFlag( Qt::AlignRight) ) {
		x = ( parentSize.width() - contentSize.width() );
	    }

	    qskSetItemGeometry( m_data->contentBox,
				x, -y,
				parentSize.width(), contentSize.height());
	    break;
	}

    case Qt::Edge::BottomEdge:
	{
	    qreal x = 0;
	    qreal y = metric( faderAspect() ) * contentSize.height() + parentSize.height() - contentSize.height();

	    if( alignment().testFlag( Qt::AlignCenter) ) {
		x = ( parentSize.width() - contentSize.width() ) / 2;
	    } else if( alignment().testFlag( Qt::AlignRight) ) {
		x = ( parentSize.width() - contentSize.width() );
	    }

	    qskSetItemGeometry( m_data->contentBox,
				x, y,
				parentSize.width(), contentSize.height());
	    break;
	}
	break;

    }

    Inherited::updateLayout();
}

void QskDrawer::aboutToShow()
{
    startTransition( Panel | QskAspect::Metric,
		     animationHint( Panel | QskAspect::Position ),
		     1.0, 0.0 );
    Inherited::aboutToShow();
}

#include "moc_QskDrawer.cpp"
