#include "QskTreeView.h"

QSK_SUBCONTROL( QskTreeView, Cell )
QSK_SUBCONTROL( QskTreeView, Text )

QskTreeView::QskTreeView( QQuickItem* const parent )
    : Inherited( parent )
{
    setSubcontrolProxy( Inherited::Cell, Cell );
    setSubcontrolProxy( Inherited::Text, Text );
}