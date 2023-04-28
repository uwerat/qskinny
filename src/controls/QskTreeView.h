#pragma once

#include <QskListView.h>

class QSK_EXPORT QskTreeView : public QskListView
{
    using Inherited = QskListView;

  public:
    QSK_SUBCONTROLS( Cell, Text )

    explicit QskTreeView( QQuickItem* const parent = nullptr );
};
