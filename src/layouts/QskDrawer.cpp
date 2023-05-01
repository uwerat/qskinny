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
};

QskDrawer::QskDrawer( QQuickItem* parentItem ):
    Inherited ( parentItem )
    , m_data( new PrivateData { } )
{
    setZ( 1 );

    setPopupFlag( PopupFlag::CloseOnPressOutside, true );

    m_data->contentBox = new QskBox(this);
    m_data->contentBox->setSubcontrolProxy( QskBox::Panel, Panel );
    m_data->contentBox->setPanel( true );

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

void QskDrawer::setEdge( Qt::Edge edge ) {
    if( m_data->edge == edge )
    {
        return;
    }

    update();
    m_data->edge = edge;
    edgeChanged( edge );
}

void QskDrawer::setContent( QskControl* content ) {
    content->setParentItem( m_data->contentBox );
    m_data->content = content;
}

void QskDrawer::updateLayout() {
    const auto& padding = paddingHint( Panel );
    const auto& contentSize = m_data->content->preferredSize()
            .grownBy( padding );
    const auto& parentSize = parentItem()->size();

    switch( m_data->edge ) {
        case Qt::Edge::LeftEdge:
    {
        qreal x = metric( faderAspect() ) * contentSize.width() * -1.0;

        qskSetItemGeometry( m_data->contentBox,
            x, 0,
            contentSize.width(), parentSize.height() );
        break;
    }
        case Qt::Edge::RightEdge:
    {
        qreal x = ( metric( faderAspect() ) * contentSize.width() )
            + parentSize.width()
            - contentSize.width();

        qskSetItemGeometry( m_data->contentBox,
            x, 0,
            contentSize.width(), parentSize.height() );
        break;
    }

        case Qt::Edge::TopEdge:
    {
        qreal y = metric( faderAspect() ) * contentSize.height();

        qskSetItemGeometry( m_data->contentBox,
            0, -y,
            parentSize.width(), contentSize.height() );
        break;
    }

        case Qt::Edge::BottomEdge:
    {
        qreal y = metric( faderAspect() ) * contentSize.height() + parentSize.height() -
            contentSize.height();

        qskSetItemGeometry( m_data->contentBox,
            0, y,
            parentSize.width(), contentSize.height() );
        break;
    }
    }
    m_data->content->setGeometry( QPointF( padding.left(), padding.top() ),
        m_data->contentBox->size().shrunkBy( padding ) );

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
